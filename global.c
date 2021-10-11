#include "general.h"

const command cmdTable[] =
{
	{"mov", 2, MOV, 0},
	{"cmp", 2, CMP, 0},
	{"add", 2, ADD, 10},
	{"sub", 2, SUB, 11},
	{"lea", 2, LEA, 0},
	{"clr", 1, CLR, 10},
	{"not", 1, NOT, 11},
	{"inc", 1, INC, 12},
	{"dec", 1, DEC, 13},
	{"jmp", 1, JMP, 10},
	{"bne", 1, BNE, 11},
	{"jsr", 1, JSR, 12},
	{"red", 1, RED, 0},
	{"prn", 1, PRN, 0},
	{"rts", 0, RTS, 0},
	{"stop", 0, STOP, 0}
};
