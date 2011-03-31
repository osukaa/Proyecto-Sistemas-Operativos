/*Implementa el primer algoritmo de exclusion mutua: Alternancia estricta*/
#include<dos.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<conio.h>
#include<iostream.h>

void interrupt myTimer(...);
void interrupt (*prev)(...);

struct process{
	char id;
	int quantum;
	int status;
	int stcPtr;
	int offset;
};

typedef int semaforo;

/*Variables globales*/
const int N = 100;
const int quantum = 4;
process pcb[2];
int indexProcess = 0;
int stackPointerAux;
int stackPointer;
int indexOffset;
int indexAux;
int quantumProcess;
int actualProcess;
int bloqueoforzado;
//variables para el uso del productor-consumidor
int buffer[N]
int i = 0;
int j = 0;
semaforo mutex = 1;
semaforo vacio = N;
semaforo lleno = 0;

void wait(semaforo *s)
{
	if (s > 0)
	{
		s = s - 1;
	}
	else
	{
		bloqueoforzado = 1;
	}
}

void signal(semaforo *s)
{
	if (s <= 0)
	{
		//quitar un proceso del pcb y ponerlo en listo
	}
	else
	{
		s = s + 1;
	}
}

void producir_elemento(int *elemento)
{
	elemento = (elemento + 1) % N;
	cprintf("producido %d",elemento);
}

void consumir_elemento(int *elemento)
{
	cprintf("consumido %d",elemento);
}

/*Procesos a controlar*/
void productor(...)
{
	int elemento = 0;
	while(1)
	{
		producir_elemento(&elemento);
		wait(&vacio);
		wait(&mutex);
			buffer[j] = elemento;
			j = (j + 1) % N;
		signal(&mutex);
		signal(&lleno);
	}
}

void consumidor(...)
{
	int elemento = 0;
	while(1)
	{
		wait(&lleno);
		wait(&mutex);
			elemento = buffer[i];
			i = (i + 1) % N;
		signal(&mutex);
		signal(&vacio);
		consumir_elemento(&elemento);
	}
}

/*inicializar el pcb*/
void initPCB(...)
{
	//Inicializa el primer nodo del PCB
	pcb[0].offset = FP_OFF(productor);
	pcb[0].quantum = quantum;
	pcb[0].id = 'A';
	pcb[0].status = 1;
	pcb[0].stcPtr = 0;
	
	//Inicializa el segundo nodo del PCB
	pcb[1].offset = FP_OFF(consumidor);
	pcb[1].quantum = quantum;
	pcb[1].id = 'B';
	pcb[1].status = 2;
	
	//Guarda el SP de nuestro programa
	asm mov stackPointer,sp
	stackPointerAux = stackPointer;
	
	//Realiza un corrimiento en la pila para el primer proceso
	stackPointerAux = stackPointerAux - 512;
	indexOffset = pcb[1].offset; //Direccion del proceso i
	
	//Guarda todo el contexto del proceso 1
	asm {
		mov SP, stackPointerAux
		pushf
		push cs
		push indexOffset
		push ax
		push bx
		push cx
		push dx
		push es
		push ds
		push si
		push di
		push bp
		mov stackPointerAux, SP
		mov SP, stackPointer
	};
	//Guarda en el PCB el SP donde se encuentran el contexto del proceso
	pcb[1].stcPtr = stackPointerAux;
	
	indexProcess = 0;
	quantumProcess =pcb[indexProcess].quantum;
}

/*Procemiento que reemplaza la interrupcion del timer con nuestro
codigo fuente.*/
void main()
{
	clrscr();
	initPCB();
	prev=getvect(8);	//Guarda la interrupciÂ¢n antigua del timer
	setvect(8,myTimer);	//Inserta con nuestro cÂ¢digo la interrupcion del time
	actualProcess = 1;
	processA();
	clrscr();
	while(1)
	{}
}
/*Codigo fuente de nuestra interrupcion del timer*/
void interrupt myTimer(...)
{
	disable();		//Apaga las demás interrupciones
	(*prev)();		//Llama al antiguo c¢digo de interrupcion
	//Espera una tecla y verifica si es la tecla Enter
	if(kbhit()  && getch() == 0xD)
	{
		//Devuelve la interrupción original
		setvect(8,prev);
		exit(0);
	}
	if (quantumProcess > 0 || bloqueoforzado == 0)
	{
		//Si el proceso aun  tiene quantum lo disminuye.
		quantumProcess--;
	}
	else
	{
		//Salva el SP del proceso que se quedo sin quantum
		asm mov  stackPointer, SP
		pcb[indexProcess].stcPtr = stackPointer;
		if (pcb[indexProcess].status == 1)
		{
			pcb[indexProcess].status = 2;
		}
		//Cambio de proceso.
		indexAux = indexProcess;
		indexProcess++;
		indexProcess = indexProcess % 2;
		if (pcb[indexProcess].status != 2)
		{
			indexProcess++;
			indexProcess = indexProcess % 2;
		}
		//Guarda el quantum del nuevo proceso.
		quantumProcess = pcb[indexProcess].quantum;

		//revisa que no se repita el proceso que se acaba de quedar sin quantum
		if (indexAux != indexProcess)
		{
			//Le indica que el nuevo proceso va estar en estado ejecutado.
			pcb[indexProcess].status = 1;
			stackPointer = pcb[indexProcess].stcPtr;
			//Mueve el SP a donde esta el contexto del nuevo proceso que va ejecutar
			asm mov sp,stackPointer
		}
	}
	enable();               //Activa las demás interrupciones
}