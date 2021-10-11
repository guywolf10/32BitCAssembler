#include "general.h"


/*this file is an implementation of the second pass of a two pass assembler*/


/*
name:		secondPass
type:		int
returns:	number of errors
parameters:	FILE *asFile:				the assembly file to execute the second pass on
		symbol *head:				the head of the symbol table
		memoryCell machineCodeImage[]:	the machine code image array
algorithm:	works according to the algorithm found in the course booklet
*/
int secondPass(FILE *asFile, symbol *head, symbol *extHead, memoryCell machineCodeImage[])
{
	int lineNum, addressIndex = 0, errors = 0;
	
	FILE *file = asFile;
	
	/*for each line in the source file*/
	for (lineNum = 1; !feof(asFile); lineNum++)
	{
		/*strings to store the line and tokens*/
		char line[MAX_LINE_LEN] = "\0";
		char token[MAX_TOKEN_LEN] = "\0";
		
		/*stores the number of tokens, their location in the line, and their length*/
		int tokenNum, tokenLoc[MAX_TOKENS_NUMBER], tokenLen[MAX_TOKENS_NUMBER];
		
		int i, commandIndex, currToken = 0, lineErrors = 0, address = 0;
		
		/*reads the next line from the source file, 1nd step of the algorithm*/
		fgets(line, MAX_LINE_LEN, file);
		
		/*disassembeling the line to individual tokens*/
		tokenNum = tokenizer(line, tokenLoc, tokenLen);
		
		/*if empty line of whitecharacter line*/
		if (!tokenNum)
			continue;
		
		/*if comment line*/
		if (line[tokenLoc[0]] == ';')
			continue;
		
		/*advancing to the first token*/
		strcpy(token, line + tokenLoc[currToken]);
		token[tokenLen[currToken]] = '\0';
		
		/*skipping the first token if it is a lable, 2nd step of the algorithm*/
		currToken += isLable(token);
		strcpy(token, line + tokenLoc[currToken]);
		token[tokenLen[currToken]] = '\0';
		
		/*moving to the next line if the command is .extern because we took care of it in the first pass, 3rd step of the algorithm*/
		if (!strcmp(token, ".extern"))
			continue;
		
		/*counting the number of lines the command will take us if it's .data*/
		if (!strcmp(token, ".data"))
		{
			/*for each token*/
			while (currToken < tokenNum - 1)
			{
				/*moving to the next token*/
				currToken++;
				strcpy(token, line + tokenLoc[currToken]);
				token[tokenLen[currToken]] = '\0';
				
				/*if the token isn't a comma it takes up a memory cell, and we will increment addressIndex*/
				if (strcmp(token, ","))
					addressIndex++;
			}
			
			continue;
		}
		
		/*counting the number of lines the command will take us if it's .string*/
		if (!strcmp(token, ".string"))
		{
			/*moving to the next token- the string*/
			currToken++;
			strcpy(token, line + tokenLoc[currToken]);
			token[tokenLen[currToken]] = '\0';
			
			/*the amount of lines is the number of characters in the string plus 1 for '\0', and the string contains 2 '\"' characters*/
			addressIndex += strlen(token) - 2 + 1;
			continue;
		}
		
		/*checking if the command is .entry, 4th step of the algorithm*/
		if (!strcmp(token, ".entry"))
		{
			/*moving to the next token to get the operand of the command*/
			currToken++;
			strcpy(token, line + tokenLoc[currToken]);
			token[tokenLen[currToken]] = '\0';
			
			/*if the symbol was inserted to the symbol table as an external symbol*/
			if (!strcmp(getAttributes(head, token), "external"))
			{
				printf("error in line %d: the symbol \"%s\" was used as an operand for the .entry command, but it's an external symbol\n", lineNum, token);
				errors++;
			}
			
			/*trying to add the attribute "entry" to the lable and printing an error if it doesn't exist in the symbol table, 5th step of the algorithm*/
			lineErrors = addEnt(head, token);
			if (lineErrors)
			{
				printf("error in line %d: the symbol \"%s\" was used as an operand for the .entry command, but it doesn't exist in the symbol table\n", lineNum, token);
				errors++;
			}
			
			continue;
		}
		
		
		
		/*if we reached this part of the code the line has a command, and it is stored inside token*/
		
		
		
		/*finding the command in the command table*/
		commandIndex = getCommandIndex(token);
		
		
		
		/*analyzing the operands and completing the final machine code translation, 6th step of the algorithm*/
		
		
		
		/*the command takes at least one machine code line*/
		addressIndex++;
		
		/*no operand commands*/
		if (cmdTable[commandIndex].numOfOperands == 0)
		{
			/*we already finished coding these in the first pass*/
			continue;
		}
		
		/*moving to the next token, the first operand*/
		currToken++;
		strcpy(token, line + tokenLoc[currToken]);
		token[tokenLen[currToken]] = '\0';
		
		/*the addressing method of the first operand*/
		address = findAddressingMethod(token);
		
		switch (address)
		{
			case DIRECT_ADDRESS:
				machineCodeImage[addressIndex].address = addressIndex + FIRST_MEMORY_ADDRESS;
				machineCodeImage[addressIndex].value = getAddress(head, token);
				machineCodeImage[addressIndex].data = 0;
				/*if the lable was declared in another file*/
				if (!strcmp(getAttributes(head, token), "external"))
				{
					machineCodeImage[addressIndex].A_R_E = 'E';
					addSymbol(extHead, token, "external", addressIndex + FIRST_MEMORY_ADDRESS);
				}
				else
					machineCodeImage[addressIndex].A_R_E = 'R';
				
				addressIndex++;
				break;
			
			case RELATIVE_ADDRESS:
				/*getting rid of the '%' char at the beginning*/
				for (i = 0; i < MAX_LINE_LEN-1; i++)
					token[i] = token[i+1];
				
				/*if the lable is declared in another file*/
				if (!strcmp(getAttributes(head, token), "external"))
				{
					printf("error in line %d: an external lable was used for relative addressing, the operand is %s\n", lineNum, token);
					errors++;
					continue;
				}
				
				machineCodeImage[addressIndex].address = addressIndex + FIRST_MEMORY_ADDRESS;
				machineCodeImage[addressIndex].value = getAddress(head, token) - machineCodeImage[addressIndex].address;
				machineCodeImage[addressIndex].data = 0;
				machineCodeImage[addressIndex].A_R_E = 'A';
				
				addressIndex++;
				break;
			
			/*if it's immediate or direct register addressing we already coded the first operand's word in the first pass*/
			default:
				addressIndex++;
				break;
		}
		
		/*2-operand commands*/
		if (cmdTable[commandIndex].numOfOperands == 2)
		{
			/*moving to the second operand, skipping the comma between them*/
			currToken += 2;
			strcpy(token, line + tokenLoc[currToken]);
			token[tokenLen[currToken]] = '\0';
			
			/*the addressing method of the second operand*/
			address = findAddressingMethod(token);
			
			switch (address)
			{
				case DIRECT_ADDRESS:
					machineCodeImage[addressIndex].address = addressIndex + FIRST_MEMORY_ADDRESS;
					machineCodeImage[addressIndex].value = getAddress(head, token);
					machineCodeImage[addressIndex].data = 0;
					/*if the lable was declared in another file*/
					if (!strcmp(getAttributes(head, token), "external"))
					{
						machineCodeImage[addressIndex].A_R_E = 'E';
						addSymbol(extHead, token, "external", addressIndex + FIRST_MEMORY_ADDRESS);
					}
					else
						machineCodeImage[addressIndex].A_R_E = 'R';
					
					addressIndex++;
					break;
				
				/*if we already coded the word in the first pass*/
				default:
					addressIndex++;
					break;
			}
		}
	}
	
	/*returning to the assembling function after reading ever line from the source file, 7th step of the algorithm*/
	return errors;
}
