#include<dos.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<conio.h>

void interrupt myTimer(...);
void interrupt (*prev)(...);
char far *scr=(char far*)0xB0008000L;
int ticks;
int k=0;
int j=0;
char str[]="oscar|alfredo";

/*Procemiento que reemplaza la interrupcion del timer con nuestro
c¢digo fuente.*/
void main()
{
	clrscr();
	prev=getvect(8);	//Guarda la interrupci¢n antigua del timer
	setvect(8,myTimer);		//Inserta con nuestro c¢digo la interrupcion del timer
	getch();		//Espera un caracter
}

/*C¢digo fuente de nuestra interrupcion del timer*/
void interrupt myTimer(...)
{
	disable();		//Apaga las dem s interrupciones
	(*prev)();		//Llama al antiguo c¢digo de interrupcion
	enable();               //Activa las dem s interrupciones
	ticks++;
	if(ticks==5)
	{
		*(scr+k*2)=str[j];
		j++;
		k=k+1;
		if(j==15)
		{
			clrscr();
			k=0;
			j=0;
		}
		ticks=1;
	}
}
