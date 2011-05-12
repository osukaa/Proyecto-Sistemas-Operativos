#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

FILE *hddFile;
char *mode = "r";
char hdd[] = "hdd.so";
long int indexPosition = 13107201;
long int indexRows = 0;

void insertIndex(char * programName, long int  programSize)
{
	char buffer[24];
	int tamano = 5;
	long int aux = indexPosition+ programSize;
	char readLine[23];
	hddFile = fopen(hdd, "r+");
	if (hddFile == NULL)
	{
		fprintf(stderr, "Error, no se encuentra el archivo del disco duro.\n");
	}
	else
	{
		fseek(hddFile, indexRows, SEEK_SET);
		fprintf(hddFile,"%s|%d|%d\n$$",programName,indexPosition,aux);
		fclose(hddFile);
		tamano += strlen(programName);
		itoa(indexPosition,buffer,10);
		tamano += strlen(buffer);
		itoa(aux,buffer,10);
		tamano += strlen(buffer);
		indexRows += tamano;
		indexPosition += programSize;
	}
}

void install(char originFile[])
{
	FILE * installFile;
	char line;
	long int fileSize = 0;
	//Obtiene la posicion donde va instalar el programa
	installFile = fopen(originFile,"r");
	hddFile = fopen("hdd.so", "r+");
	fseek(hddFile, indexPosition, SEEK_SET);
	//poner contenido del archivo instalador
	while(fscanf(installFile,"%c",&line) != EOF)
	{
		fprintf(hddFile,"%c",line);
		fileSize++;
	}
	fclose(hddFile);
	fclose(installFile);
	line = 0;
	insertIndex(originFile, fileSize);
}

void initHDD()
{
	long int i;
	hddFile = fopen(hdd,"w");
	for(i = 0; i < 52428801; i++)
	{
		fprintf(hddFile,"%c",'%');
	}
	fseek(hddFile, 0, SEEK_SET);
	fprintf(hddFile,"%s","$$");
	fclose(hddFile);
}

main()
{
	char fileName[10];
	clrscr();
	initHDD();
	printf("Desea instalar un nuevo programa?\n");
	while(getch() == 0x79)
	{
		printf("Ingrese un archivo a instalar: ");
		fgets(fileName,9,stdin);
		install(fileName);
		printf("Instalacion completa\n");
		printf("Desea instalar un nuevo programa?\n");
		getch();
	}
	return 0;
}
