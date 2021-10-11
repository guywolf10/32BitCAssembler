#include "general.h"


/*this file is an implementation of the output files building and printing of a two-pass assembler*/


/*
name:		createAndPrintFiles
type:		void
returns:	none
parameters:	char *fileName:			the name of the source file
		symbol *head:				the head of the symbol table
		memoryCell machineCodeImage[]:	the machine code image array
		int ICF:				the final instruction counter
		int DCF:				the final data counter
		int entFlag:				a flag to tell whether or not to create an entry file
		int externFlag:			a flag to tell whether or not to create an external file
algorithhm:	opens the necessary files and calls to other functions to print to them
*/
void createAndPrintFiles(char *fileName, symbol *head, symbol *extHead, memoryCell machineCodeImage[], int ICF, int DCF, int entFlag, int externFlag)
{
	/*enough room for the name of the file + the extension ".ent" + '\0'*/
	char *name = (char *)malloc((strlen(fileName) + 5) * sizeof(char));
	
	FILE *objFile;
	
	/*printing the object file*/
	strcpy(name, fileName);
	strcat(name, ".ob");
	objFile = fopen(name, "w");
	printObj(objFile, machineCodeImage, ICF, DCF);
	fclose(objFile);
	
	/*printing the entry file if we need to*/
	if (entFlag)
	{
		FILE *entFile;
		
		strcpy(name, fileName);
		strcat(name, ".ent");
		entFile = fopen(name, "w");
		printEnt(entFile, head);
		fclose(entFile);
	}
	
	/*printing the external file if we need to*/
	if (externFlag)
	{
		FILE *externFile;
		
		strcpy(name, fileName);
		strcat(name, ".ext");
		externFile = fopen(name, "w");
		printExt(externFile, extHead);
		fclose(externFile);
	}
	
	free(name);
}


/*
name:		printObj
type:		void
returns:	none
parameters:	FILE *file:				the file to print to
		memoryCell machineCodeImage[]:	the machine code image array
		int ICF:				the number of code lines
		int DCF:				the number of data lines
algorithm:	prints the machine code according to the instructions found in the course booklet
*/
void printObj(FILE *file, memoryCell machineCodeImage[], int ICF, int DCF)
{
	int i;
	
	/*printing the headline*/
	fprintf(file, "   %d %d\n", ICF - FIRST_MEMORY_ADDRESS, DCF);
	
	/*printing the code*/
	for (i = 0; i < ICF + DCF - FIRST_MEMORY_ADDRESS; i++)
		if (!machineCodeImage[i].data)
			fprintf(file, "%04d %03X %c\n", machineCodeImage[i].address, (machineCodeImage[i].value & 0xfff), machineCodeImage[i].A_R_E);
	
	/*printing the data*/
	for (i = 0; i < ICF+DCF; i++)
		if (machineCodeImage[i].data)
			fprintf(file, "%04d %03X %c\n", machineCodeImage[i].address, (machineCodeImage[i].value & 0xfff), machineCodeImage[i].A_R_E);
}


/*
name:		printEnt
type:		void
returns:	none
parameters:	FILE *file:	the file to print to
		symbol *head:	the head of the symbol table
algorithm:	goes over the symbol table and prints the symbol with the "entry" attribute
*/
void printEnt(FILE *file, symbol *head)
{
	symbol *temp;
	
	for (temp = head; temp != NULL; temp = temp->next)
		if (!strcmp(temp->attributes, "code, entry") || !strcmp(temp->attributes, "data, entry"))
			fprintf(file, "%s %04d\n", temp->name, temp->value);
}


/*
name:		printExt
type:		void
returns:	none
parameters:	FILE *file:	the file to print to
		symbol *head:	the head of the symbol table
algorithm:	goes over the symbol table and prints the symbol with the "external" attribute
*/
void printExt(FILE *file, symbol *extHead)
{
	symbol *temp;
	
	for (temp = extHead; temp != NULL; temp = temp->next)
		fprintf(file, "%s %04d\n", temp->name, temp->value);
}
