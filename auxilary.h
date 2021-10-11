/*libraries*/


/*macros*/


/*global variables*/


/*structs and typedefs*/
typedef struct
{
	int address;
	int value : 12;
	unsigned int data : 1; /*data or code*/
	char A_R_E;
} memoryCell;


/*function prototypes*/
int tokenizer(char *, int [], int []);	/*a function to separate the line into individual tokens*/
int isLable(char *);				/*a function to check if a string is a legal lable name*/
int getCommandIndex(char *);			/*a function to find the command in the command table*/
int machineCodeMasking(int, int, int, int);	/*a function to translate a command to machine code*/
int findAddressingMethod(char *);		/*a function to find the addressing method of an operand*/
int isZero(char *);				/*a function to determine if a string is a representation of the number 0*/
int findRegisterNum(char *);			/*a function to find the number of a register from a string*/
void incrementArr(memoryCell [], int, int);	/*a function to increment the address of all data lines*/
