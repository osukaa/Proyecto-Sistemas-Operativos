#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <limits.h>

FILE *hddFile;
char *mode = "r";
char hdd[] = "hdd.so";
int indexPosition = 0;

void insertIndex(char * programName, long int programFinale)
{
	int flag = -1;
	int position = 0;
	char readLine[23];
	char final[] = "$$";
	char * programInfo;
	char * aux;
	hddFile = fopen(hdd, "r+");
	while(flag != 0)
	{
		fscanf(hddFile,"%s",&readLine);
		flag = strcmp(readLine, final);
		if(flag != 0)
		{
			position++;
		}
	}
	fseek(hddFile, position, SEEK_SET);
	fprintf(hddFile,"%s|%d|%d\n$$",programName,position,programFinale);
	fclose(hddFile);
}

int index()
{
	char sline[23];
	char aux[23];
	char final[] = "$$";
	int flag = -1;
	char * finalPos;
	long int finalPosition = 0;
	//Abre el archivo del disco duro
	hddFile = fopen(hdd, mode);
	if (hddFile == NULL)
	{
		fprintf(stderr, "Error, no se encuentra el archivo del disco duro.\n");
	}
	else
	{
		//Buscar final del indice
		while(flag != 0)
		{
			fscanf(hddFile,"%s",&sline);
			flag = strcmp(sline, final);
			if(flag != 0)
			{
				strcpy(aux,sline);
				indexPosition = indexPosition + strlen(sline);
			}
		}
		fclose(hddFile);
		//hace un split para obtener donde termina el ultimo programa
		finalPos = strtok(aux,"|");
		while(finalPos != NULL)
		{
			strcpy(aux, finalPos);
			finalPos = strtok(NULL,"|");
		}
		finalPosition = atol(aux);
	}
	return finalPosition;
}

void install(char originFile[])
{
	long int finalProgram;
	FILE * installFile;
	char line;
	long int fileSize = 0;
	//Obtiene la posicion donde va instalar el programa
	finalProgram = index();
	finalProgram += 13107201;
	installFile = fopen(originFile,"r");
	hddFile = fopen("hdd.so", "a+");
	fseek(hddFile, finalProgram, SEEK_SET);
	//poner contenido del archivo instalador
	while(fscanf(installFile,"%c",&line) != EOF)
	{
		fprintf(hddFile,"%c",line);
		fileSize++;
	}
	fclose(hddFile);
	fclose(installFile);
	line = 0;
	insertIndex("file",fileSize);
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
	initHDD();
	printf("termino ciclo\n");
	install("file.exe");
	printf("Termino la instalacion\n");
	getch();
	return 0;
}
