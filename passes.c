#include "general.h"


/*
name:		assemble
type:		int
returns:	1 if there were errors while trying to assemble the file, or 0 otherwise
parameters:	char *fileName:		the name of the file to assemble
		symbol *head:			the head of the symbol table
		memoryCell machineCodeImage:	the machine code image array
		int *entryFlag:		a flag for if there were ".entry" commands
		int *externalFlag:		a flag for if there were ".extern" commands
algorithm:	tries to execute the first and second passes, and if it encounters errors it stops and returns 1
*/
int assemble(char *fileName, symbol *head, symbol *extHead, memoryCell machineCodeImage[], int *ICF, int *DCF, int *entryFlag, int *externFlag)
{
	/*an error counter*/
	int errors = 0;
	
	/*the name of the assembly file, the length of the name + 3 characters for the extension ".as"*/
	char *name = (char *)malloc((strlen(fileName) + 3) * sizeof(char));
	
	/*the assembly file*/
	FILE *asFile;
	
	strcpy(name, fileName);
	strcat(name, ".as");
	
	asFile = fopen(name, "r");
	
	/*if the file doesn't exist or couldn't be opened*/
	if (asFile == NULL)
	{
		printf("error: couldn't open the file %s, moving to the next file\n", name);
		
		free(name);
		
		return 1;
	}
	
	/*trying to execute the first pass*/
	errors = firstPass(asFile, head, machineCodeImage, ICF, DCF, entryFlag, externFlag);
	
	/*if there were errors in the first pass*/
	if (errors)
	{
		printf("\n\n\nthere were errors in the first pass, stopping now and moving to the next file\n\n\n");
		
		fclose(asFile);
		free(name);
		
		return 1;
	}
	
	/*rewinding the file to the beginning*/
	rewind(asFile);
	
	/*trying to execute the second pass*/
	errors = secondPass(asFile, head, extHead, machineCodeImage);
	
	/*if there were errors in the second pass*/
	if (errors)
	{
		printf("\n\n\nthere were errors in the second pass, stopping now and moving to the next file\n\n\n");
		
		fclose(asFile);
		free(name);
		
		return 1;
	}
	
	/*if there weren't any errors in the first and second passes*/
	return 0;
}
