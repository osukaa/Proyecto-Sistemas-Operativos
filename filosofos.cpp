#include<dos.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<conio.h>
#include<iostream.h>
#define izq = (i - 1) % N
#define der = (i + 1) % N

const int N = 5;

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

process pcb[N];
int indexProcess = 0;
int stackPointerAux;
int stackPointer;
int indexOffset;
int indexAux;
int quantumProcess;
int actualProcess;
typedef int semaforo;
enum { pensando, hambriento, comiendo };

int estado[N];
semaforo mutex = 1;
semaforo s[N];

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
		wait(s);
	}
}

void signal(semaforo *s)
{
	*s = *s + 1;
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

void prueba(int i)
{
	if(estado[i] == hambriento && estado[izq] != comiendo && estado[der] != comiendo)
	{
		estado[i] = comiendo;
		signal(&s[i]);
	}
}

void tomar_tenedores(int i)
{
	wait(&exmut);
	estado[i] = hambriento;
	prueba(i);
	signal(&exmut);
	wait(&s[i]);
}

void dejar_tenedores(int i)
{
	wait(&exmut);
	estado[i] = pensando;
	prueba(izq);
	prueba(der);
	signal(&exmut);
}

void filosofo1()
{
	while(1)
	{
		pensar();
		tomar_tenedores(0);
		comer();
		dejar_tenedores(0);
	}
}
void filosofo2()
{
	while(1)
	{
		pensar();
		tomar_tenedores(1);
		comer();
		dejar_tenedores(1);
	}
}
void filosofo3()
{
	while(1)
	{
		pensar();
		tomar_tenedores(2);
		comer();
		dejar_tenedores(2);
	}
}
void filosofo4()
{
	while(1)
	{
		pensar();
		tomar_tenedores(3);
		comer();
		dejar_tenedores(3);
	}
}
void filosofo5()
{
	while(1)
	{
		pensar();
		tomar_tenedores(4);
		comer();
		dejar_tenedores(4);
	}
}

void initPCB(...)

{

	//Inicializa el primer nodo del PCB
	pcb[0].offset = FP_OFF(filosofo1);
	pcb[0].quantum = quantum;
	pcb[0].id = 'A';
	pcb[0].status = 1;
	pcb[0].stcPtr = 0;

	//Inicializa el segundo nodo del PCB
	pcb[1].offset = FP_OFF(filosofo2);
	pcb[1].quantum = quantum;
	pcb[1].id = 'B';
	pcb[1].status = 2;

	//Inicializa el tercer nodo del PCB
	pcb[2].offset = FP_OFF(filosofo3);
	pcb[2].quantum = quantum;
	pcb[2].id = 'C';
	pcb[2].status = 2;

	//Inicializa el cuarto nodo del PCB
	pcb[3].offset = FP_OFF(filosofo4);
	pcb[3].quantum = quantum;
	pcb[3].id = 'D';
	pcb[3].status = 2;
	
	pcb[3].offset = FP_OFF(filosofo5);
	pcb[3].quantum = quantum;
	pcb[3].id = 'E';
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

	stackPointerAux = stackPointerAux - 2048;
	indexOffset = pcb[4].offset;

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
	pcb[4].stcPtr = stackPointerAux;
	indexProcess = 0;

	quantumProcess = pcb[indexProcess].quantum;
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
	if (quantumProcess > 0 && pcb[indexProcess].status == 1)
	{
		quantumProcess--;
	}
	else
	{
		if ( pcb[indexProcess].status == 1)
		{
			pcb[indexProcess].status == 2;
		}
		//Salva el SP del proceso que se quedo sin quantum
		asm mov stackPointer, SP
		pcb[indexProcess].stcPtr = stackPointer;
		indexProcess = (indexProcess + 1) % M;
		stackPointer = pcb[indexProcess].stcPtr;
		quantumProcess = pcb[indexProcess].quantum;
		//Mueve el SP a donde esta el contexto del nuevo proceso que va ejecutar
		asm mov sp,stackPointer
		//Cambio de proceso.
		f = 0;
	}
	enable(); //Activa las demás interrupciones
}