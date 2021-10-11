#include "general.h"


/*
name:		tokenizer
type:		int
returns:	the number of tokens in the line
parameters:	char *line:	the line to disassemble to individual tokens
		int starts[]:	the array to store where the individual tokens start
		int lengths[]:	the array to store the length of each token
algorithm:	reads every token separately and stores the starting index in the line and the length of the token for each token
*/
int tokenizer(char *line, int starts[], int lengths[])
{
	int i = 0, currToken = -1, lineLen = strlen(line);
	
	/*if the line is empty*/
	if (!lineLen)
		return 0;
	
	/*for each token*/
	do
	{
		/*initializing the arrays in the index of the current token*/
		currToken++;
		starts[currToken] = 0;
		lengths[currToken] = 0;
		
		/*skipping whitecharacters*/
		for (; i < lineLen && (line[i] == ' ' || line[i] == '\t'); i++);
		
		/*if we haven't reached the end of the line and there is a token to read*/
		if (i < lineLen)
		{
			
			/*if it's a newline character which means it's the end of the line*/
			if (line[i] == '\n')
				return currToken;
			
			/*if it's a new token, we will store it's location in the starts array*/
			starts[currToken] = i;
			
			/*if it's a comma we will treat it as it's own token*/
			if (line[i] == ',')
			{
				lengths[currToken] = 1;
				i++;
				continue;
			}
			
			/*if it's the beginning of a string*/
			if (line[i] == '\"')
			{
				lengths[currToken]++;
				i++;
				
				while (i < lineLen)
				{
					lengths[currToken]++;
					i++;
					
					/*if we reached the end of the string*/
					if (line[i-1] == '\"')
						break;
				}
			}
			
			/*if we are not reading a string*/
			else
			{
				/*while the token isn't finished we will increment its saved length*/
				while (i < lineLen && line[i] != ',' && line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
				{
					lengths[currToken]++;
					i++;
				}
				
			}
		}
		
	} while (i < lineLen);
	
	/*after finishing reading the line, we return the number of tokens*/
	return currToken;
}


/*
name:		isLable
type:		int
returns:	1 if the string is a valid lable name, 0 otherwise
parameters:	char *name:	the string to validate
algorithm:	compares the string to saved words and checks if it's a lable based on the requirements in the course booklet. also trims the colon at the end of the lable (if it has a colon)
*/
int isLable(char *name)
{
	int i;
	
	/*if it's longer than the max length of a lable*/
	if (strlen(name) > MAX_SYMBOL_LEN)
		return 0;
	
	/*if the first character isn't an alphabetic letter or the string doesn't end with a colon*/
	if (!isalpha(name[0]) || name[strlen(name)-1] != ':')
		return 0;
	
	/*trimming the colon at the end*/
	name[strlen(name)-1] = '\0';
	
	/*if there is a non-alphabetic-non-digit character*/
	for (i = 0; i < strlen(name); i++)
		if (!isalpha(name[i]) && !isdigit(name[i]))
			return 0;
	
	/*if it is a register's name*/
	if(strlen(name) == 2 && name[0] == 'r' && name[1] - '0' >= 0 && name[1] - '0' <= REGISTER_NUM)
		return 0;
	
	/*if it is a command name*/
	for (i = 0; i < NUM_OF_COMMANDS; i++)
		if (!strcmp(name, cmdTable[i].name))
			return 0;
	
	/*if it is one of the 4 guidence line commands*/
	if (!strcmp(name, ".data") || !strcmp(name, ".string") || !strcmp(name, ".entry") || !strcmp(name, ".extern"))
		return 0;
	
	/*if the name is valid*/
	return 1;
}


/*
name:		getCommandIndex
type:		int
returns:	the index of the command in the command array, or -1 if it doesn't exist
parameters:	char *command:	the command we are looking for
algorithm:	going over the command array looking for the command, if we find it we return the index, if we don't we return -1
*/
int getCommandIndex(char *command)
{
	int i;
	
	/*going over the command array looking for the string*/
	for(i = 0; i < NUM_OF_COMMANDS; i++)
		if(!strcmp(command, cmdTable[i].name))
			return i;
	
	/*if we haven't found the command*/
	return -1;
}


/*
name:		machineCodeMasking
type:		int
returns:	the command translated to machine code
parameters:	int opcode:			the opcode field of the command
		int funct:			the funct field of the command
		int sourceAddress:		the addressing method of the command's source operand
		int destinationAddress:	the addressing method of the command's destination operand
algorithm:	initializes the value to 0, and for each operand it masks it and shifts it to make room for the next operand
*/
int machineCodeMasking(int opcode, int funct, int sourceAddress, int destinationAddress)
{
	int machineCodeLine;
	machineCodeLine = 0;
	
	/*masking the appropriate fields*/
	machineCodeLine |= opcode;
	machineCodeLine = machineCodeLine << 4;
	machineCodeLine |= funct;
	machineCodeLine = machineCodeLine << 2;
	machineCodeLine |= sourceAddress;
	machineCodeLine = machineCodeLine << 2;
	machineCodeLine |= destinationAddress;
	
	return machineCodeLine;
}


/*
name:		findAddressingMethod
type:		int
returns:	the addressing method of the given operand
parameters:	char *operand:	the operand to return the addressing method of
algorithm:	compares to register names, checks if it starts with '#', checks if it stars with %, and returns the corresponding addressing method
*/
int findAddressingMethod(char *operand)
{
	/*checks if it starts with '#', the immediate number addressing method*/
	if (operand[0] == '#')
		return IMMEDIATE_ADDRESS;
	
	/*checks if it starts with '%', the relative addressing method*/
	if (operand[0] == '%')
		return RELATIVE_ADDRESS;
	
	/*checks if it's a register name, the direct register addressing method*/
	if (strlen(operand) == 2 && operand[0] == 'r' && (operand[1] == '0' || operand[1] == '1' || operand[1] == '2' || operand[1] == '3' || operand[1] == '4' || operand[1] == '5' || operand[1] == '6' || operand[1] == '7'))
		return REGISTER_ADDRESS;
	
	/*by process of elimination, the addressing method is direct addressing*/
	return DIRECT_ADDRESS;
}


/*
name:		isZero
type:		int
returns:	1 if a string is a representation of the number 0, 0 otherwise.
parameters:	char *str:	the string to check if it represents the number 0
algorithm:	checks if the string doesn't start with '0' or a +/- sign, has more than one '.' or characters other than '0'
*/
int isZero(char *str)
{
	int i, periodNum = 0;
	
	/*if the string has an illegal start*/
	if (str[0] != '+' && str[0] != '-')
	{
		if (str[0] != '0')
		{
			return 0;
		}
	}
	
	else
	{
		if (str[1] != 0)
		{
			return 0;
		}
	}
	
	/*now checking if it has illegal characters in the middle*/
	for (i = 1; i < strlen(str) && str[i] != '\n'; i++)
	{
		if (str[i] != '0')
		{
			if (str[i] == '.')
			{
				/*if there is more than one '.'*/
				if (periodNum)
					return 0;
				
				else
					periodNum++;
			}
			
			/*if the character isn't '0' nor '.'*/
			else
				return 0;
		}
	}
	
	/*if we reached the end of the string without errors*/
	return 1;
}


/*
name:		findRegisterNum
type:		int
returns:	the register's number
parameters:	char *reg:	the string representing a register
algorithm:	checks the second character of the string and returns the corrosponding number
*/
int findRegisterNum(char *reg)
{
	return (reg[1] - '0');
}


/*
name:		incrementArr
type:		void
returns:	none
parameters:	memoryCell machineCodeImage:	the machine code image array
		int ICF:			the number of code lines and the address to start the data image from
		int totalLines:		the number of total lines in the array, used to know how many lines to go over
algorithm:	checks if the memory cell holds data instead of code and if it does, increments it by ICF
*/
void incrementArr(memoryCell machineCodeImage[], int ICF, int totalLines)
{
	int i;
	
	for (i = 0; i < totalLines; i++)
		/*if the memory cell holds data and not code*/
		if (machineCodeImage[i].data)
			machineCodeImage[i].address += ICF;
}
