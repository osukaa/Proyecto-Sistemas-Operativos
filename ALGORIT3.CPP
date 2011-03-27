/*Implementa el segundo algoritmo de exclusion mutua:
Colisión región crítica no garantiza la exclusión mutua*/
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

/*Variables globales*/
process pcb[2];
const int quantum = 4;
int indexProcess = 0;
int stackPointerAux;
int stackPointer;
int indexOffset;
int indexAux;
int quantumProcess;
int p1DeseaEntrar;
int p2DeseaEntrar;
/*Procesos a controlar*/
void processA(...)
{
	while(1)
	{
		//tareas previas uno
		p1DeseaEntrar = 1;
		while(p2DeseaEntrar == 1)
		{};
		//seccion critica
		gotoxy(2,2);
		cprintf("proceso A");
		p1DeseaEntrar = 0;
		cprintf("termino A");
		//otras tareas
	};
}

void processB(...)
{
	while(1)
	{
		//tareas previas dos
		p2DeseaEntrar = 1;
		while(p1DeseaEntrar == 1)
		{};
		//Seccion critica dos
		gotoxy(2,4);
		cprintf("proceso b");
		p2DeseaEntrar = 0;
		cprintf("termino b");
		//otras tareas
	};
}

/*inicializar el pcb*/
void initPCB(...)
{
	//Inicializa el primer nodo del PCB
	pcb[0].offset = FP_OFF(processA);
	pcb[0].quantum = quantum;
	pcb[0].id = 'A';
	pcb[0].status = 1;
	pcb[0].stcPtr = 0;

	//Inicializa el segundo nodo del PCB
	pcb[1].offset = FP_OFF(processB);
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
	prev=getvect(8);        //Guarda la interrupci¢n antigua del timer
	setvect(8,myTimer);     //Inserta con nuestro c¢digo la interrupcion del time
	p1DeseaEntrar = 0;
	p2DeseaEntrar = 0;
	processA();
	clrscr();
	while(1)
	{};
}
/*Codigo fuente de nuestra interrupcion del timer*/
void interrupt myTimer(...)
{
	disable();              //Apaga las demas interrupciones
	(*prev)();              //Llama al antiguo codigo de interrupcion
	//Espera una tecla y verifica si es la tecla Enter
	if(kbhit()  && getch() == 0xD)
	{
		//Devuelve la interrupci?n original
		setvect(8,prev);
		exit(0);
	}
	if (quantumProcess > 0)
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
	enable();               //Activa las dem?s interrupciones
}
