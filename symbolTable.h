/*libraries*/


/*macros*/


/*global variables*/


/*structs and typedefs*/
struct node
{
	char *name;
	char *attributes;
	int value;
	struct node *next;
};

typedef struct node symbol;


/*function prototypes*/
symbol *newTable();				/*creates a new empty list of symbol*/
int addSymbol(symbol *, char *, char *, int);	/*adds a new symbol to the table, returns 1 if it already exists and 0 otherwise*/
int addEnt(symbol *, char *);			/*adds "entry" to the type of the symbol with symbolName as name*/
int getAddress(symbol *, char *);		/*gets the address of a symbol*/
void incrementTable(symbol *, int);		/*adds ICF to every symbol with type "data"*/
void freeTable(symbol *);			/*frees all the allocated memory of the symbol table*/
char *getAttributes(symbol *, char *);	/*gets the attributes of a symbol*/
