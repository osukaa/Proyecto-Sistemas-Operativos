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

/*Variables globales*/
process pcb[2];
const int quantum = 4;
int indexProcess = 0;
int stackPointerAux;
int stackPointer;
int indexOffset;
int indexAux;
int quantumProcess;
int actualProcess;

/*Procesos a controlar*/
void processA(...)
{
	int color = 1;
	while(1)
	{
		//tareas previas
		while (actualProcess == 2)
		//seccion critica
		gotoxy(2,2);
		textcolor(color);
		cprintf("soy proceso A");
		actualProcess = 2;
		//otras tareas
		color++;
		color = color % 15;
	}
}

void processB(...)
{
	int color = 15;
	while(1)
	{
		//tareas previas
		while (actualProcess == 1)
		//Seccion critica
		gotoxy(2,4);
		textcolor(color);
		cprintf("soy proceso B");
		actualProcess = 1;
		//Otras tareas
		color++;
		color = color % 15;
	}
}

void processC(...)
{
	while(1)
	{
		gotoxy(2,6);
		cprintf("soy C =) ");
	}
}

void processD(...)
{
	while(1)
	{
		gotoxy(2,8);
		cprintf("soy D ;) ");
	}
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
		//copia el valor que posee el quantum del proceso actual
		int q = pcb[indexProcess].quantum;
		if (q > 20)
		{ 
			//Si el quantum del proceso es mayor a 20 reinicia el quantum en su valor original
			q = quantum;
		}
		else
		{
			/*Si el quantum del proceso actual no es mayor a 20 le aumenta uno para que a la siguiente ejecución
			tenga un tick de reloj más*/
			q++;
		}
		//Asigna el nuevo quantum al proceso.
		pcb[indexProcess].quantum = q;
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