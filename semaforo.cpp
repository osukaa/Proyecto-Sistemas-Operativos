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
	//1 ejecucion, 2 listo, 3 bloqueado
	int status;
	int stcPtr;
	int offset;
};

typedef int semaforo;
/*Variables globales*/
const int N = 3;
const int M = 4;
const int quantum = 5;
process pcb[M];
int indexProcess = 0;
int stackPointerAux;
int stackPointer;
int indexOffset;
int indexAux;
int quantumProcess;
int actualProcess;
//variables para el uso del productor-consumidor
int buffer[N];
int i = 0;
int j = 0;
int f = 1;
semaforo mutex = 1;
semaforo vacio = N;
semaforo lleno = 0;
//semaforo mutexc = 1;
//semaforo mutexp = 1;

void wait(semaforo *s)
{
	if (*s > 0)
	{
		*s = *s - 1;
	}
	else
	{
		f = 1;
		pcb[indexProcess].status = 3;
		while ( f == 1 ) {};
	}
}

void signal(semaforo *s)
{
	*s=*s+1;
	//quitar un proceso del pcb y ponerlo en listo
	int tmp = (indexProcess+1) % M;
	int i = 0;
	int v = 0;
	while (i == 0 && v < 4)
	{
		if (pcb[tmp].status == 3)
		{
			pcb[tmp].status = 2;
			i = 1;
		}
		tmp = (tmp + 1) % M;
		v++;
	}
}

void producir_elemento(int *elemento, int aux)
{
	disable();
	*elemento = (*elemento + 1) % N;
	//gotoxy(0, j);
	printf("\nproductor %d inserto %d en pos %d|",aux,*elemento,j);
	delay(1000);
	enable();
}

void consumir_elemento(int *elemento, int aux)
{
	disable();
	//gotoxy(70, i);
	printf("\nconsumidor %d extrajo %d en pos %d|",aux,*elemento,i);
	delay(1000);
	enable();
}

/*Procesos a controlar*/
void productorA(...)
{
	int elemento = 0;
	while(1)
	{
		//wait(&mutexp);
		wait(&vacio);
		wait(&mutex);
		disable();
			producir_elemento(&elemento,1);
			buffer[j] = elemento;
			j = (j + 1) % N;
		signal(&mutex);
		signal(&lleno);
		enable();
		//signal(&mutexp);
	}
}

void consumidorA(...)
{
	int elemento = 1;
	while(1)
	{
		//wait(&mutexc);
		wait(&lleno);
		wait(&mutex);
		disable();
			elemento = buffer[i];
			consumir_elemento(&elemento,1);
			i = (i + 1) % N;
		signal(&mutex);
		signal(&vacio);
		enable();
		//signal(&mutexc);

	}
}

void productorB(...)
{
	int elemento = 0;
	while(1)
	{
		//wait(&mutexp);
		wait(&vacio);
		wait(&mutex);
		disable();
			buffer[j] = elemento;
			producir_elemento(&elemento,2);
			j = (j + 1) % N;
		signal(&mutex);
		signal(&lleno);
		enable();
		//signal(&mutexp);
	}
}

void consumidorB(...)
{
	int elemento = 1;
	while(1)
	{
		//wait(&mutexc);
		wait(&lleno);
		wait(&mutex);
		disable();
			elemento = buffer[i];
			consumir_elemento(&elemento,2);
			i = (i + 1) % N;
		signal(&mutex);
		signal(&vacio);
		enable();
		//signal(&mutexc);
	}
}
/*inicializar el pcb*/
void initPCB(...)

{

	//Inicializa el primer nodo del PCB
	pcb[0].offset = FP_OFF(productorA);
	pcb[0].quantum = quantum;
	pcb[0].id = 'A';
	pcb[0].status = 1;
	pcb[0].stcPtr = 0;

	//Inicializa el segundo nodo del PCB
	pcb[1].offset = FP_OFF(consumidorA);
	pcb[1].quantum = quantum;
	pcb[1].id = 'B';
	pcb[1].status = 2;

	//Inicializa el tercer nodo del PCB
	pcb[2].offset = FP_OFF(productorB);
	pcb[2].quantum = quantum;
	pcb[2].id = 'C';
	pcb[2].status = 2;

	//Inicializa el cuarto nodo del PCB
	pcb[3].offset = FP_OFF(consumidorB);
	pcb[3].quantum = quantum;
	pcb[3].id = 'D';
	pcb[3].status = 2;

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

	//Realiza un corrimiento en la pila para el segundo proceso
	stackPointerAux = stackPointerAux - 1024;
	indexOffset = pcb[2].offset;

	//Guarda todo el contexto del proceso 2
	asm {
		mov sp,stackPointerAux
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
	pcb[2].stcPtr = stackPointerAux;

	//Realiza un corrimiento en la pila para el tercer proceso

	stackPointerAux = stackPointerAux - 1536;
	indexOffset = pcb[3].offset;

	//Guarda todo el contexto del proceso 3
	asm {
		mov sp,stackPointerAux
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
	pcb[3].stcPtr = stackPointerAux;
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
	productorA();
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
	if (pcb[indexProcess].status == 3)
	{
		//Salva el SP del proceso que se quedo sin quantum
		asm mov stackPointer, SP
		pcb[indexProcess].stcPtr = stackPointer;
		indexProcess = (indexProcess + 1) % M;
		stackPointer = pcb[indexProcess].stcPtr;
		//Mueve el SP a donde esta el contexto del nuevo proceso que va ejecutar
		asm mov sp,stackPointer
		//Cambio de proceso.
		f = 0;
	}
	enable(); //Activa las demás interrupciones
}