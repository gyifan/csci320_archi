// CSCI 320 - Computer Architecture
// Lab 4 - Soreboard
// Yifan Ge
// 9/24/2013
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define FLOAT 	0
#define INT	1
#define INSTR_SIZE		10
#define PROGRAM_SIZE		1000
#define INTREG			32
#define FLOATREG		16
#define MAX_INSTR_LENGTH	1000
#define MAX_OPCODE_LENGTH	10
#define PROGRAM			"../data/sample.code"
#define MAX_CLK_CYCLES	100000

#define INTEGER_UNIT		0;
#define MULT_UNIT		1;
#define ADD_UNIT		2;
#define DIV_UNIT		3;

typedef struct{
	int type; 	// either FLOAT or INT
	int write;	// stores the instr index that is writing to it. initialized as -1
	int read;	// stores the instr index that is reading it. if multiple instruction
			// is reading it, it will restore the highest index. initialized as -1
} Reg;

typedef struct{
	int updateRD;   // finished reading, need update.
	int updateWR;	// just finished writeback, need update.
	int done;	// initialized as 0;
	int issued;	// initialized as 0;
	int* fu; 	// Function unit
	Reg* rd;	// Destination register
	Reg* rs1;	// Source register 1
	Reg* rs2;	// Source register 2
	int counterEX;	// initiated as -1, counts how many clocks in EX
	int clockEX;	// clock needed for this executing instruction
	int waitIndex;	// instruction index this instruction is waiting. Default(-1)
	int issue;	// clock cycle issued, default -1
	int read;	// clock cycle read operands, default -1
	int ex;		// clock cycle executed operands, default -1
	int wb;		// clack cycle wroteback, default -1
	char name[20];	// stores the name of the instruction;
} Instr;

