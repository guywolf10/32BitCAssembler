#include "general.h"


/*this file is an implementation of the first pass of a two pass assembler*/


/*
name:		firstPass
type:		int
returns:	number of errors
parameters:	FILE *asFile:				the assembly file to execute the first pass on
		symbol *head:				the head of the symbol table
		memoryCell machineCodeImage[]:	the machine code image array
		int *ICF:				the final insctruction counter
		int *DCF:				the final data counter
		int *entFlag:				a flag to turn on if there are any entry commands
		int *externFlag:			a flag to turn on if there are any extern commands
algorithm:	works according to the algorithm found in the course booklet
*/
int firstPass(FILE *asFile, symbol *head, memoryCell machineCodeImage[], int *ICF, int *DCF, int *entFlag, int *externFlag)
{
	/*initializing IC and DC, 1st step of the algorithm*/
	int DC = 0, IC = FIRST_MEMORY_ADDRESS;
	
	int lineNum, addressIndex = 0, errors = 0;
	
	FILE *file = asFile;
	
	/*for each line in the source file*/
	for (lineNum = 1; !feof(asFile); lineNum++)
	{
		/*strings to store the line and tokens*/
		char line[MAX_LINE_LEN] = "\0";
		char token[MAX_TOKEN_LEN] = "\0";
		char lableToken[MAX_TOKEN_LEN] = "\0";
		
		/*stores the number of tokens, their location in the line, and their length*/
		int tokenNum, tokenLoc[MAX_TOKENS_NUMBER], tokenLen[MAX_TOKENS_NUMBER];
		
		int commandIndex, currToken = 0, lableFlag = 0, lineErrors = 0, address = 0, secondAddress = 0, numOfLines = 0, num = 0;
		
		/*reads the next line from the source file, 2nd step of the algorithm*/
		fgets(line, MAX_LINE_LEN, file);
		
		/*disassembeling the line to individual tokens*/
		tokenNum = tokenizer(line, tokenLoc, tokenLen);
		
		/*if empty line of whitecharacter line*/
		if (!tokenNum)
			continue;
		
		/*if comment line*/
		if (line[tokenLoc[0]] == ';')
			continue;
		
		/*storing the first token in a separate string for easy access*/
		strcpy(lableToken, line + tokenLoc[0]);
		lableToken[tokenLen[0]] = '\0';
		
		/*if there is an illegal comma*/
		if (token[0] == ';')
		{
			printf("error in line %d: there is an illegal comma\n", lineNum);
			errors++;
			continue;
		}
		
		/*checking to see if the first token is a lable and turning on a flag if it is, 3rd and 4th steps of the algorithm*/
		lableFlag = isLable(lableToken);
		
		/*if there is an empty lable declaration*/
		if (lableFlag && tokenNum == 1)
		{
			printf("error in line %d: empty lable definition\n", lineNum);
			errors++;
			continue;
		}
		
		/*if there is an illegal comma*/
		if (lableToken[0] == ',')
		{
			printf("error in line %d: there is an illegal comma\n", lineNum);
			errors++;
			continue;
		}
		
		/*advancing to the first non-lable token*/
		currToken = lableFlag;
		strcpy(token, line + tokenLoc[currToken]);
		token[tokenLen[currToken]] = '\0';
		
		/*checking if the first non-lable token is a data storage command, 5th step of the algorithm*/
		if (!strcmp(token, ".data") || !strcmp(token, ".string"))
		{
			/*if there are not enough tokens in the line*/
			if (tokenNum < lableFlag+2)
			{
				printf("error in line %d: empty data storage command\n", lineNum);
				errors++;
				continue;
			}
			
			if (lableFlag)
			{
				/*trying to insert the lable to the symbol table and checking for errors, 6th step of the algorithm*/
				lineErrors = addSymbol(head, lableToken, "data", DC);
				if (lineErrors)
				{
					printf("error in line %d: the lable \"%s\" already exists in the symbol table\n", lineNum, lableToken);
					errors++;
					continue;
				}
			}
			
			/*if the command is .data*/
			if (!strcmp(token, ".data"))
			{
				int i, legalComma;
				
				/*for each token*/
				for (i = 0; currToken < tokenNum-1; i++)
				{
					legalComma = i % 2;	/*only expecting comma once very two tokens*/
					
					/*moving to the next token*/
					currToken++;
					strncpy(token, line + tokenLoc[currToken], tokenLen[currToken]);
					token[tokenLen[currToken]] = '\0';
					
					/*if there is a missing comma*/
					if (legalComma && token[0] != ',')
					{
						errors++;
						printf("error in line %d: missing comma\n", lineNum);
						break;
					}
					
					/*if we are expecting data*/
					if (!legalComma)
					{
						/*if there is an illegal comma*/
						if (token[0] == ';')
						{
							printf("error in line %d: there is an illegal comma\n", lineNum);
							errors++;
							continue;
						}
						
						/*converting the data to an integer*/
						num = atoi(token);
						
						/*if the data isn't a legal number*/
						if (!num && !isZero(token))
						{
							printf("error: an illegal operand was used as data in line %d\n", lineNum);
							errors++;
							break;
						}
						
						/*checking if the number could be represented with only 12 bits*/
						if (num < -2048 || num > 2047)
						{
							printf("error in line %d: a number outside range of representation with 12 bit 2's complement was used as data\n", lineNum);
							errors++;
							break;
						}
						
						/*inserting the number into the machine code image array, 7th step of the algorithm*/
						machineCodeImage[addressIndex].address = DC;
						machineCodeImage[addressIndex].value = num;
						machineCodeImage[addressIndex].data = 1;
						machineCodeImage[addressIndex].A_R_E = 'A';
						
						addressIndex++;
						DC++;
					}
				}
				
				/*if the last token in the line was a comma*/
				if (legalComma)
				{
					printf("error in line %d: the last token of the line was a comma and there was no data after it in line\n", lineNum);
					errors++;
					break;
				}
			}
			
			
			/*if the command is .string*/
			else if (!strcmp(token, ".string"))
			{
				/*an index used to iterate the string*/
				int j;
				
				/*if there are too many tokens in the line- more than the command, the operand, and a possible lable*/
				if (tokenNum > lableFlag+2)
				{
					printf("error in line %d: too many tokens\n", lineNum);
					errors++;
					continue;
				}
				
				/*moving on to the next token, the string*/
				currToken++;
				strncpy(token, line + tokenLoc[currToken], tokenLen[currToken]);
				token[tokenLen[currToken]] = '\0';
				
				/*if the string doesn't start with '"'*/
				if (token[0] != '"')
				{
					printf("error in line %d: the string %s doesn't start with \'\"\'\n", lineNum, token);
					errors++;
					break;
				}
				
				/*if the string doesn't end with '"'*/
				if (token[strlen(token)-1] != '"' && (token[strlen(token)-1] != '\n' && token[strlen(token)-2] != '"'))
				{
					printf("error: the string in line %d doesn't end with \'\"\"\n", lineNum);
					errors++;
					break;
				}
				
				/*inserting the characters of the string to the machine code image array, 7th step of the algorithm*/
				for (j = 1; j < tokenLen[currToken]-1; j++)
				{
					machineCodeImage[addressIndex].address = DC;
					machineCodeImage[addressIndex].value = (int)token[j];
					machineCodeImage[addressIndex].data = 1;
					machineCodeImage[addressIndex].A_R_E = 'A';
					
					addressIndex++;
					DC++;
				}
				
				/*inserting the null character at the end of the string*/
				machineCodeImage[addressIndex].address = DC;
				machineCodeImage[addressIndex].value = (int)'\0';
				machineCodeImage[addressIndex].data = 1;
				machineCodeImage[addressIndex].A_R_E = 'A';
				
				addressIndex++;
				DC++;
			}
			
			/*we finished reading this line, so we are moving to the next one*/
			continue;
		}
		
		/*if the command isn't a data storage command but still one of the 4 guideance line commands, 8th step of the algorithm*/
		if (!strcmp(token, ".entry") || !strcmp(token, ".extern"))
		{
			/*saving the command*/
			char commandToken[MAX_TOKEN_LEN];
			strcpy(commandToken, token);
			
			/*if there are not enough tokens in the line*/
			if (tokenNum < lableFlag+2)
			{
				printf("error in line %d: not enough tokens\n", lineNum);
				errors++;
				continue;
			}
			
			/*if there are too many tokens in the line- more than the command, the operand, and a possible lable*/
			if (tokenNum > lableFlag+2)
			{
				printf("error in line %d: too many tokens\n", lineNum);
				errors++;
				continue;
			}
			
			/*getting the lable's name*/
			currToken++;
			strncpy(token, line + tokenLoc[currToken], tokenLen[currToken]);
			token[tokenLen[currToken]] = '\0';
			
			/*adding ':' at the end of the lable's name in order to use the function isLable to check if it is a valid lable name, the function will trim the ':' off*/
			strcat(token, ":");
			
			/*if it's not a valid lable name*/
			if (!isLable(token))
			{
				printf("error in line %d: the commands .entry and .extern expects a lable's name as the operand, but the operand %s isn't a valid lable name\n", lineNum, token);
				errors++;
				continue;
			}
			
			/*if it's a .entry command we go back to step two, the command will be taken care of in the second pass, 9th step of the algorithm*/
			if (!strcmp(commandToken, ".entry"))
				/*turning on a flag to know whether to create an entry file later on, we won't create it if there were errors*/
				entFlag[0] = 1;
			
			/*if it's a .extern command we try to insert the operand to the symbol table and check for errors, 10th step of the algorithm*/
			else if (!strcmp(commandToken, ".extern"))
			{
				/*turning on a flag to know whether to create an extern file later on, we won't create it if there were errors*/
				externFlag[0] = 1;
				
				lineErrors = addSymbol(head, token, "external", 0);
				
				if (lineErrors)
				{
					printf("error in line %d: the lable \"%s\" already exists in the symbol table without the \"external\" attribute\n", lineNum, token);
					errors++;
				}
			}
			
			/*going back to step 2- reading the next line*/
			continue;
		}
		
		
		
		/*if we reached this part of the code the line has a command, and it is stored inside token*/
		
		
		
		if (lableFlag)
		{
			/*trying to insert the lable to the symbol table and checking for errors, 11th step of the algorithm*/
			lineErrors = addSymbol(head, lableToken, "code", IC);
			if (lineErrors)
			{
				printf("error in line %d: the lable \"%s\" already exists in the symbol table\n", lineNum, lableToken);
				errors++;
				continue;
			}
		}
		
		/*finding the command in the command table and printing an error message if it doesn't exist, 12th step of the algorithm*/
		commandIndex = getCommandIndex(token);
		if (commandIndex == -1)
		{
			printf("error in line %d: the command \"%s\" doesn't exist\n", lineNum, token);
			errors++;
			continue;
		}
		
		
		
		/*starting to analyze the commands and the operands to see how many additional memory cells we will need, 13th step of the algorithm*/
		
		
		
		/*the base number of memory cells we will need is one*/
		numOfLines = 1;
		
		/*no-operand commands*/
		if (cmdTable[commandIndex].numOfOperands == 0)
		{
			/*if there are too many token in the line- more than the command and a possible lable*/
			if (tokenNum > lableFlag+1)
			{
				printf("error in line %d: too many operands for the no-operand command %s\n", lineNum, cmdTable[commandIndex].name);
				errors++;
				continue;
			}
			
			address = NO_OPERAND;
			secondAddress = NO_OPERAND;
		}
		
		/*one-operand or two-operand commands*/
		else
		{
			/*used to trim the '#' char in case of immediate addressing*/
			int i;
			
			/*if there are not enough tokens in the line*/
			if (tokenNum < lableFlag+2)
			{
				printf("error in line %d: not enough tokens\n", lineNum);
				errors++;
				continue;
			}
			
			/*advancing to the first operand*/
			currToken++;
			strncpy(token, line + tokenLoc[currToken], tokenLen[currToken]);
			token[tokenLen[currToken]] = '\0';
			
			/*if there is an illegal comma*/
			if (token[0] == ';')
			{
				printf("error in line %d: there is an illegal comma\n", lineNum);
				errors++;
				continue;
			}
			
			/*finding the addressing method of the first operand*/
			address = findAddressingMethod(token);
			
			/*one-operand commands*/
			if (cmdTable[commandIndex].numOfOperands == 1)
			{
				/*if there are too many tokens in the line- more than the command, the one operand and a possible lable*/
				if (tokenNum > lableFlag+2)
				{
					printf("error in line %d: too many operands for the one-operand command %s\n", lineNum, cmdTable[commandIndex].name);
					errors++;
					continue;
				}
				
				/*the first operand is actually the destination operand, so we will move address into secondAddress and put NO_OPERAND in address*/
				secondAddress = address;
				address = NO_OPERAND;
				
				/*checking how many additional memory cells we will need, and coding it if possible*/
				switch (secondAddress)
				{
					case IMMEDIATE_ADDRESS:
						/*if the use of immediate addressing isn't allowed with the command*/
						if (cmdTable[commandIndex].opcode != PRN)
						{
							printf("error in line %d: illegal use of immediate addressing with the command %s, the operand is %s\n", lineNum, cmdTable[commandIndex].name, token);
							errors++;
							continue;
						}
						
						/*trimming the '#' char at the beginning to covert the operand to int*/
						for (i = 0; i < MAX_TOKEN_LEN-1; i++)
							token[i] = token[i+1];
						
						/*converting the operand to int*/
						num = atoi(token);
						
						/*if the operand isn't a legal number*/
						if (!num && !isZero(token))
						{
							printf("error in line %d: the operand #%s isn't a legal representation of a number and was used for immediate addressing\n", lineNum, token);
							errors++;
							continue;
						}
						
						/*checking if the number could be represented with only 12 bits*/
						if (num < -2048 || num > 2047)
						{
							printf("error in line %d: a number outside range of representation with 12 bit 2's complement was used for immediate addressing\n", lineNum);
							errors++;
							continue;
						}
						
						/*coding the number in the machine code image array, 14, step of the algorithm (note: leaving room for the first word which we will code later)*/
						machineCodeImage[addressIndex+numOfLines].address = IC+numOfLines;
						machineCodeImage[addressIndex+numOfLines].value = num;
						machineCodeImage[addressIndex+numOfLines].data = 0;
						machineCodeImage[addressIndex+numOfLines].A_R_E = 'A';
						
						/*we will need an additional memory cell for the immediate value*/
						numOfLines++;
						break;
					
					/*note: all one-operand commands can use direct addressing for their operand*/
					case DIRECT_ADDRESS:
						machineCodeImage[addressIndex+numOfLines].address = IC+numOfLines;
						machineCodeImage[addressIndex+numOfLines].data = 0;
						
						/*we will need an additional memory cell for the address of the lable*/
						numOfLines++;
						break;
					
					case RELATIVE_ADDRESS:
						/*if the use of relative addressing isn't allowed with the command*/
						if (cmdTable[commandIndex].opcode != JMP && cmdTable[commandIndex].opcode != BNE && cmdTable[commandIndex].opcode != JSR)
						{
							printf("error in line %d: illegal use of relative addressing with the command %s, the operand is %s\n", lineNum, cmdTable[commandIndex].name, token);
							errors++;
							continue;
						}
						
						machineCodeImage[addressIndex+numOfLines].address = IC+numOfLines;
						machineCodeImage[addressIndex+numOfLines].data = 0;
						
						/*we will need an additional memory cell for the distance to the lable*/
						numOfLines++;
						break;
					
					case REGISTER_ADDRESS:
						/*if the use of direct register addressing isn't allowed with the command*/
						if (cmdTable[commandIndex].opcode == JMP || cmdTable[commandIndex].opcode == BNE || cmdTable[commandIndex].opcode == JSR)
						{
							printf("error in line %d: illegal use of direct register addressing with the command %s, the operand is %s\n", lineNum, cmdTable[commandIndex].name, token);
							errors++;
							continue;
						}
						
						/*coding the number of the register in the machine code image array, 14, step of the algorithm (note: leaving room for the first word which we will code later)*/
						machineCodeImage[addressIndex+numOfLines].address = IC+numOfLines;
						machineCodeImage[addressIndex+numOfLines].value = (1 << findRegisterNum(token));
						machineCodeImage[addressIndex+numOfLines].data = 0;
						machineCodeImage[addressIndex+numOfLines].A_R_E = 'A';
						
						/*we will need one additional memory cell for the register's number*/
						numOfLines++;
						break;
				}
			}
			
			/*two-operand commands*/
			if (cmdTable[commandIndex].numOfOperands == 2)
			{
				/*if there are too many tokens in the line- more than the command, the two operands, a comma and a possible lable*/
				if (tokenNum > lableFlag+4)
				{
					printf("error in line %d: too many operands for the two-operand command %s\n", lineNum, cmdTable[commandIndex].name);
					errors++;
					continue;
				}
				
				/*checking how many additional memory cells we will need for the first operand, and coding it if possible*/
				switch (address)
				{
					case IMMEDIATE_ADDRESS:
						/*if the use of immediate addressing isn't allowed with the command*/
						if (cmdTable[commandIndex].opcode == LEA)
						{
							printf("error in line %d: illegal use of immediate addressing with the command %s, the operand is %s\n", lineNum, cmdTable[commandIndex].name, token);
							errors++;
							continue;
						}
						
						/*trimming the '#' char at the beginning to covert the operand to int*/
						for (i = 0; i < MAX_TOKEN_LEN-1; i++)
							token[i] = token[i+1];
						
						/*converting the operand to int*/
						num = atoi(token);
						
						/*if the operand isn't a legal number*/
						if (!num && !isZero(token))
						{
							printf("error in line %d: the operand #%s isn't a legal representation of a number and was used for immediate addressing\n", lineNum, token);
							errors++;
							continue;
						}
						
						/*checking if the number could be represented with only 12 bits*/
						if (num < -2048 || num > 2047)
						{
							printf("error in line %d: a number outside range of representation with 12 bit 2's complement was used for immediate addressing\n", lineNum);
							errors++;
							continue;
						}
						
						/*coding the number in the machine code image array, 14, step of the algorithm (note: leaving room for the first word which we will code later)*/
						machineCodeImage[addressIndex+numOfLines].address = IC+numOfLines;
						machineCodeImage[addressIndex+numOfLines].value = num;
						machineCodeImage[addressIndex+numOfLines].data = 0;
						machineCodeImage[addressIndex+numOfLines].A_R_E = 'A';
						
						/*we will need an additional memory cell for the immediate value*/
						numOfLines++;
						break;
					
					/*note: all two-operand commands can use direct addressing for their first operand*/
					case DIRECT_ADDRESS:
						machineCodeImage[addressIndex+numOfLines].address = IC+numOfLines;
						machineCodeImage[addressIndex+numOfLines].data = 0;
						
						/*we will need an additional memory cell for the address of the lable*/
						numOfLines++;
						break;
					
					/*note: no two-operand command can use relative addressing for their first operand*/
					case RELATIVE_ADDRESS:
						printf("error in line %d: illegal use of relative addressing with the command %s, the operand is %s\n", lineNum, cmdTable[commandIndex].name, token);
						errors++;
						continue;
						break;
					
					case REGISTER_ADDRESS:
						/*if the use of direct register addressing isn't allowed with the command*/
						if (cmdTable[commandIndex].opcode == LEA)
						{
							printf("error in line %d: illegal use of direct register addressing with the command %s, the operand is %s\n", lineNum, cmdTable[commandIndex].name, token);
							errors++;
							continue;
						}
						
						/*coding the number of the register in the machine code image array, 14, step of the algorithm (note: leaving room for the first word which we will code later)*/
						machineCodeImage[addressIndex+numOfLines].address = IC+numOfLines;
						machineCodeImage[addressIndex+numOfLines].value = (1 << findRegisterNum(token));
						machineCodeImage[addressIndex+numOfLines].data = 0;
						machineCodeImage[addressIndex+numOfLines].A_R_E = 'A';
						
						/*we will need one additional memory cell for the register's number*/
						numOfLines++;
						break;
				}
				
				/*if there isn't a second operand nor a comma*/
				if (tokenNum < lableFlag+3)
				{
					printf("error in line %d: too little operands for the two-operand command %s\n", lineNum, cmdTable[commandIndex].name);
					errors++;
					continue;
				}
				
				/*advancing to the comma between the two operands*/
				currToken++;
				strncpy(token, line + tokenLoc[currToken], tokenLen[currToken]);
				token[tokenLen[currToken]] = '\0';
				
				/*if there is a missing comma*/
				if (token[0] != ',')
				{
					errors++;
					printf("error in line %d: missing comma\n", lineNum);
					break;
				}
				
				/*if there isn't a second operand after the comma*/
				if (tokenNum < lableFlag+4)
				{
					printf("error in line %d: too little operands for the two-operand command %s\n", lineNum, cmdTable[commandIndex].name);
					errors++;
					continue;
				}
				
				/*advancing to the second operand*/
				currToken++;
				strncpy(token, line + tokenLoc[currToken], tokenLen[currToken]);
				token[tokenLen[currToken]] = '\0';
				
				/*if there is an illegal comma*/
				if (token[0] == ';')
				{
					printf("error in line %d: there is an illegal comma\n", lineNum);
					errors++;
					continue;
				}
				
				/*the addressing method of the second operand*/
				secondAddress = findAddressingMethod(token);
				
				/*checking how many additional memory cells we will need for the second operand, and coding it if possible*/
				switch (secondAddress)
				{
					case IMMEDIATE_ADDRESS:
						/*if the use of immediate addressing isn't allowed*/
						if (cmdTable[commandIndex].opcode != CMP)
						{
							printf("error in line %d: illegal use of immediate addressing with the command %s, the operand is %s\n", lineNum, cmdTable[commandIndex].name, token);
							errors++;
							continue;
						}
						
						/*trimming the '#' char in the beginning of the operand to convert it to an integer*/
						for (i = 0; i < MAX_TOKEN_LEN-1; i++)
							token[i] = token[i+1];
						
						/*converting the number to an integer*/
						num = atoi(token);
						
						/*if the operand isn't a legal number*/
						if (!num && !isZero(token))
						{
							printf("error in line %d: illegal operand used for immediate addressing, the operand is %s\n", lineNum, token);
							errors++;
							continue;
						}
						
						/*checking if the number could be represented with only 12 bits*/
						if (num < -2048 || num > 2047)
						{
							printf("error in line %d: the number %d is outside range of representation with 12 bit 2's complement was\n", lineNum, num);
							errors++;
							continue;
						}
						
						/*inserting the number into the machine code image array, 14th step of the algorithm*/
						machineCodeImage[addressIndex+numOfLines].address = IC+numOfLines;
						machineCodeImage[addressIndex+numOfLines].value = num;
						machineCodeImage[addressIndex+numOfLines].data = 0;
						machineCodeImage[addressIndex+numOfLines].A_R_E = 'A';
						
						/*we will need an additional memory cell for the immediate value*/
						numOfLines++;
						break;
					
					/*note: all two-operand commands can use direct addressing for their destination operand*/
					case DIRECT_ADDRESS:
						machineCodeImage[addressIndex+numOfLines].address = IC+numOfLines;
						machineCodeImage[addressIndex+numOfLines].data = 0;
						
						/*we will need an additional memory cell for the address of the lable*/
						numOfLines++;
						break;
					
					/*note: no two-operand command can use relative addressing for their destination operand*/
					case RELATIVE_ADDRESS:
						printf("error in line %d: illegal use of relative addressing with the command %s, the operand is %s\n", lineNum, cmdTable[commandIndex].name, token);
						errors++;
						continue;
						break;
					
					/*note: all two-operand commands can use direct register addressing for their destination operand*/
					case REGISTER_ADDRESS:
						/*inserting the register into the machine code image array, 14th step of the algorithm*/
						machineCodeImage[addressIndex+numOfLines].address = IC+numOfLines;
						machineCodeImage[addressIndex+numOfLines].value = 1 << findRegisterNum(token);
						machineCodeImage[addressIndex+numOfLines].data = 0;
						machineCodeImage[addressIndex+numOfLines].A_R_E = 'A';
						
						/*we will need one additional memory cell for the register's number*/
						numOfLines++;
						break;
				}
			}
		}
		
		/*coding the first word into the machine code image array, 14th step of the algorithm*/
		machineCodeImage[addressIndex].address = IC;
		machineCodeImage[addressIndex].value = machineCodeMasking(cmdTable[commandIndex].opcode, cmdTable[commandIndex].funct, address, secondAddress);
		machineCodeImage[addressIndex].data = 0;
		machineCodeImage[addressIndex].A_R_E = 'A';
		
		/*incrementing the address index and IC by the number of lines we needed to code/skip for the command*/
		addressIndex += numOfLines;
		IC += numOfLines;
		
		/*note: the 15th step of the algorithm isn't relevent to my implementation of the assembler*/
		
		/*returning to the beginning of the loop, 16th step of the algorithm*/
	}
	
	
	
	/*this section of the code is reached after reading all of the file*/
	
	
	
	/*note: step 17 of the algorithm isn't relevant to my implementation of the first pass*/
	
	/*saving IC and DC as ICF and DCF so we will be able to use them from the main function, 18th step of the algorithm*/
	ICF[0] = IC;
	DCF[0] = DC;
	
	/*incrementing every symbol with the "data" attribute by ICF, 19th step of the algorithm*/
	incrementTable(head, IC);
	
	/*incrementing every line in the machine code array that represents data by ICF to seperate the code and data images*/
	incrementArr(machineCodeImage, IC, addressIndex);
	
	/*returning to the main function to start the second pass, 20 step of the algorithm*/
	return errors;
}
