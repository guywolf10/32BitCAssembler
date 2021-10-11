#include "general.h"


/*this file is an implementation of the symbol table of a two pass assembler*/


/*
name:		createList
type:		symbol *
returns:	a pointer to a new symbol list
parameters:	none
algorithm:	allocates memory and initializes value to an impossible value so we can tell it's an empty list
*/
symbol *newTable()
{
	symbol *head = (symbol *)calloc(1, sizeof(symbol));
	
	head->name = (char *)calloc(1, sizeof(char) * MAX_SYMBOL_LEN);
	head->attributes = (char *)calloc(1, sizeof(char) * MAX_SYMBOL_LEN);
	head->value = -1;
	head->next = NULL;
	
	return head;
}


/*
name:		addSymbol
type:		int
returns:	0 if there weren't any errors or 1 if the symbol already exists
parameters:	symbol *head:		the head of the symbol table
		char *newName:		the name of the symbol to add
		char *newAttributes:	the attributes of the symbol to add
		int newValue:		the value of the symbol to add
algorithm:	if the head is NULL we initialize it, if it isn't we go to the end of the table
		to add a new one, and check every symbol on the way to check if the symbol already exists
*/
int addSymbol(symbol *head, char *newName, char *newAttributes, int newValue)
{
	/*if the list is empty, we will change the head instead of creating a new node*/
	if (head->value == -1)
	{
		strcpy(head->name, newName);
		strcpy(head->attributes, newAttributes);
		head->value = newValue;
	}
	
	/*if the list isn't empty, we will create a new node and insert it to the end of the list*/
	else
	{
		symbol *newNode = (symbol *)calloc(1, sizeof(symbol));
		symbol *temp;
		
		
		char *tempName = (char *)calloc(1, sizeof(char) * MAX_SYMBOL_LEN);
		char *tempAttributes = (char *)calloc(1, sizeof(char) * MAX_SYMBOL_LEN);
		
		strcpy(tempName, newName);
		strcpy(tempAttributes, newAttributes);
		
		newNode->name = tempName;
		newNode->attributes = tempAttributes;
		newNode->value = newValue;
		newNode->next = NULL;
		
		for (temp = head; temp->next != NULL; temp = temp->next)
			/*if the symbol already exists*/
			if (!strcmp(newNode->name, temp->name))
			{
				int error = 1;
				
				/*if they don't both have the "external" attribute*/				
				if (strcmp(newNode->attributes, "external") || strcmp(temp->attributes, "external"))
					error = 1;
				
				/*freeing the memory*/
				free(tempAttributes);
				free(tempName);
				free(newNode);
				
				return error;
			}
		
		/*if there aren't any errors, adding the symbol to the end of the list*/
		temp->next = newNode;
	}
	
	return 0;
}


/*
name:		getAddress
type:		int
returns:	the address of the symbol who's name was given, or -1 if we didn't find the symbol
parameters:	symbol *head:		the head of the symbol table
		char *symbolName:	the name of the symbol we are looking for
algorithm:	goes over every symbol and if the name matches the one we are looking for we return the address, if we didn't find the symbol we return -1.
*/
int getAddress(symbol *head, char *symbolName)
{
	symbol *temp;
	
	for (temp = head; temp != NULL; temp = temp->next)
		if (!strcmp(temp->name, symbolName))
			return temp->value;
	
	return -1;
}


/*
name:		getAttributes
type:		char *
returns:	the attributes of the symbol who's name was given, or NULL if we didn't find the symbol
parameters:	symbol *head:		the head of the symbol table
		char *symbolName:	the name of the symbol we are looking for
algorithm:	goes over every symbol and if the name matches the one we are looking for we return the attributes, if we didn't find the symbol we return NULL.
*/
char *getAttributes(symbol *head, char *symbolName)
{
	symbol *temp;
	
	for (temp = head; temp != NULL; temp = temp->next)
		if (!strcmp(temp->name, symbolName))
			return temp->attributes;
	
	return NULL;
}


/*
name:		addEnt
type:		int
returns:	0 if we found the symbol or 1 if we didn't find the symbol
parameters:	symbol *head:		the head of the symbol table
		char *symbolName:	the name of the lable to add the ", entry" to
algorithm:	goes over the symbol table until it finds the symbol to add the ", entry" to or until the end of the table
*/
int addEnt(symbol *head, char *symbolName)
{
	symbol *temp;
	
	for (temp = head; temp != NULL; temp = temp->next)
		if (!strcmp(symbolName, temp->name))	/*if we found the symbol*/
		{
			strcat(temp->attributes, ", entry");
			return 0;
		}
	
	/*if we reached the end of the table and didn't find the symbol*/
	return 1;
}


/*
name:		incrementTable
type:		void
returns:	none
parameters:	symbol *head:	the head of the symbol table
		int ICF:	the number of code lines in the memory
algorithm:	goes over every symbol in the symbol table and increments it's value if it's a data type symbol
*/
void incrementTable(symbol *head, int ICF)
{
	symbol *temp = head;
	
	while (temp != NULL)
	{
		if (!strcmp(temp->attributes, "data"))
			temp->value += ICF;
		
		temp = temp->next;
	}
	
	return;
}


/*
name:		freeTable
type:		void
returns:	none
parameters:	symbol *head: the head of the symbol table
algorithm:	goes over the symbol table with a temporary variable and frees all allocated memory
*/
void freeTable(symbol *head)
{
	symbol *temp = head;
	
	while (temp != NULL)
	{
		head = head->next;
		
		free(temp->attributes);
		free(temp->name);
		free(temp);
		
		temp = head;
	}
}
