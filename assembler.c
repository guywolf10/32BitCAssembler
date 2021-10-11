#include "general.h"


/*this program is an implementation of a two pass assembler*/


/*
name:		main
type:		int
returns:	0
parameters:	int argc:	number of command line arguments
		char *argv[]:	array of command line arguments
algorithm:	the main function goes over each file given in the command line and does the following:
		if the file exists and can be opened it sends the file to the first pass of the assembler
		if the first pass didn't have any errors it then sends the file to the second pass of the assembler
		if the second pass didn't have any errors it then makes an object file and maybe an entry or an extern file, and prints the machine code as needed
*/
int main(int argc, char *argv[])
{
	int i;
	
	/*for each file given through the command line*/
	for (i = 1; i < argc; i++)
	{
		/*creating a new symbol table*/
		symbol *head = newTable();
		
		/*creating a new externals table*/
		symbol *extHead = newTable();
		
		/*creating a new machine code image array*/
		memoryCell machineCodeImage[MAX_MACHINE_CODE_LINES];
		
		/*the final instruction counter and data counter*/
		int ICF, DCF;
		
		/*flags to know whether to create files or not*/
		int entryFlag = 0, externalFlag = 0;
		
		/*trying to assemble the files*/
		int errors = assemble(argv[i], head, extHead, machineCodeImage, &ICF, &DCF, &entryFlag, &externalFlag);
		
		/*if there weren't errors while assembling*/
		if (!errors)
			/*creating the files and printing the necessary data to them*/
			createAndPrintFiles(argv[i], head, extHead, machineCodeImage, ICF, DCF, entryFlag, externalFlag);
		
		/*freeing the allocated memory*/
		freeTable(head);
		freeTable(extHead);
	}
	
	/*after going through all the files*/
	return 0;
}
