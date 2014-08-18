// CSCI 320 - Computer Architecture
// Lab 4 - Soreboard
// Yifan Ge
// 9/24/2013
//

#include "scoreboard.h"

// global instances
FILE* fd;
char* buf;
Instr* instr[PROGRAM_SIZE];
int pc;
int clock;

// registers
Reg* ireg[INTREG];
Reg* freg[FLOATREG];

// fuction unit counter & max
int intClk;
int addClk;
int multClk;
int divClk;

int intUnitCount;
int addUnitCount;
int multUnitCount;
int divUnitCount;

// counters
int i;
int j;

/*
 * initilizes the scoreboard processor
 * 	Registers, pc, clock, set other to default
 */
void init(){
	// initialize int reg
	for(i=0;i<INTREG;i++){
		ireg[i] = (Reg *) malloc(sizeof(Reg));
		ireg[i]->type = INT;
		ireg[i]->write = -1;
		ireg[i]->read = -1;
	}

	// initialize float reg
	for(i=0;i<FLOATREG;i++){
		freg[i] = (Reg *) malloc(sizeof(Reg));
		freg[i]->type = FLOAT;
		freg[i]->write = -1;
		freg[i]->read = -1;
	}

	// initialize pc and clock to 0
	pc = 0;
	clock = 0;

	// initialize the read file buffer
	buf = malloc(MAX_INSTR_LENGTH);

	// initialize default scoreboard properties;
	// integer 1 1; mult 2 10; add 1 2; div 1 40
	intClk = 1;
	addClk = 2;
	multClk = 10;
	divClk = 40;

	intUnitCount = 1;
	addUnitCount = 1;
	multUnitCount = 2;
	divUnitCount = 1;

}

/*
 * gets the integer register index by parsing "Rk" string.
 * return value: int k
 */
int getIntRegIndex(char* reg){
	return atoi(strtok(reg, "R"));
}

/*
 * gets the integer register index by parsing "i(Rk)" string.
 * return value: int k
 */
int getIntRegIndexAd(char* reg){
	char* temp;
	temp = strtok(reg, "() ");
	temp = strtok(NULL, "() ");
	return getIntRegIndex(temp);
}

/*
 * gets the float register index by parsing "Fk" string.
 * return value: int k
 */
int getFloatRegIndex(char* reg){
	return atoi(strtok(reg, "F"));
}

/*
 * pritnts all floating registers with write and read status
 * format: F[k] write: instr_index, read: instr_index
 */
void printFloatReg(){
	int k;
	for(k=0; k<FLOATREG; k++){
		printf("F[%d] write: %d, read: %d\n", k, freg[k]->write, freg[k]->read);
	}
}

/*
 * prints all integer registers with write and read status
 * format: R[k] write: instr_index, read: instr_index
 */
void printIntReg(){
	int k;
	for(k=0; k<INTREG; k++){
		printf("R[%d] write: %d, read %d\n", k, ireg[k]->write, ireg[k]->read);
	}
}

/*
 * resets all register status to default value
 * write: -1; read: -1
 */
void resetReg(){
	int k;
	for(k=0; k<INTREG; k++){
		ireg[k]->read = -1;
		ireg[k]->write = -1;
	}

	for(k=0; k<FLOATREG; k++){
		freg[k]->write = -1;
		freg[k]->read = -1;
	}
}

/*
 * prints all the fields of the instruction struct
 */
void printInstrStatus(){
	int k = 0;

	while(NULL != instr[k]){
		if(NULL==instr[k]->rs2){
			printf("instr: %d, done:%d, issued: %d, fu: %d, rd: %d, rs1: %d, counterEX: %d, clockEX: %d, issue: %d, read: %d, ex: %d, wb: %d.\n",k, instr[k]->done, instr[k]->issued, *(instr[k]->fu), instr[k]->rd->write, instr[k]->rs1->write, instr[k]->counterEX, instr[k]->clockEX, instr[k]->issue, instr[k]->read, instr[k]->ex, instr[k]->wb);
		}else{printf("instr: %d, done:%d, issued: %d, fu: %d, rd: %d, rs1: %d, rs2: %d, counterEX: %d, clockEX: %d, issue: %d, read: %d, ex: %d, wb: %d.\n",k, instr[k]->done, instr[k]->issued, *(instr[k]->fu), instr[k]->rd->write, instr[k]->rs1->write, instr[k]->rs2->write, instr[k]->counterEX, instr[k]->clockEX, instr[k]->issue, instr[k]->read, instr[k]->ex, instr[k]->wb);
		}
		k++;
	}
}

/*
 * prints the function unit usage: number of unit available
 */
void printUnitStatus(){
	printf("intUnit: %d, multUnit: %d, addUnit: %d, divUnit: %d\n", intUnitCount, multUnitCount, addUnitCount, divUnitCount); 
}

/*
 * prints the final scoreboard results
 */
void printScoreboardResult(){
	printf("\n Printing Results ... ...\n===============================================================\n");
	printf("                                  	Read		Execute		Write\n");
	printf("                            Issue	Operands	Complete	Result\n");
	printf("                      --------------------------------------------------------------\n");
	int k = 0;
	while(NULL != instr[k]){
		printf("%s		%d		%d		%d		%d\n", instr[k]->name, instr[k]->issue, instr[k]->read, instr[k]->ex, instr[k]->wb);
		k++;
	}
}

/* 
 * loads the program, sets the instruction lists
 */
void loadProgram(char* fileLoc){

	printf("Loading program %s ... ... \n", PROGRAM);
	// open the program file
	if(NULL==(fd=fopen(fileLoc,"r"))){
		fprintf(stderr, "Open program file failed\n");
		exit(-1);
	}

	// initializes the counter
	i = 0; // used for looping the tokens;
	j = 0; // used for counting the instruction index;
	char *token[8];
	char temp[MAX_INSTR_LENGTH];


	// parse the program into instructions 
	while (NULL!=(fgets(buf, MAX_INSTR_LENGTH, fd))){
		strcpy(temp, buf);
		temp[strcspn(temp,"\n")]='\0';
		token[0] = strtok(buf, " ,");
		while(token[i] != NULL){
			i++;
			token[i] = strtok(NULL, " ,");
		}

		// parse and initialize scoreboard properties.
		// set fileds: functional unit counts and execution clock cycles
		if(0==strncmp(token[0],".integer", MAX_OPCODE_LENGTH)){
			intUnitCount = atoi(token[1]);
			intClk = atoi(token[2]);
		}else if(0==strncmp(token[0],".mult", MAX_OPCODE_LENGTH)){
			multUnitCount = atoi(token[1]);
			multClk = atoi(token[2]);
		}else if(0==strncmp(token[0],".add", MAX_OPCODE_LENGTH)){
			addUnitCount = atoi(token[1]);
			addClk = atoi(token[2]);
		}else if(0==strncmp(token[0],".div", MAX_OPCODE_LENGTH)){
			divUnitCount = atoi(token[1]);
			divClk = atoi(token[2]);
		}

		/*
		 * program instructions parsing
		 * instruction implemented: 
		 * LD 		Fi, i(Rk)
		 * ADDD 	Fi, Fj, Fk
		 * SUBD 	Fi, Fj, Fk
		 * MULTD 	Fi, Fj, Fk
		 * DIVD		Fi, Fj, Fk
		 */
		else if(0==strncmp(token[0],"LI", MAX_OPCODE_LENGTH)){
		}else if(0==strncmp(token[0],"LW", MAX_OPCODE_LENGTH)){
		}else if(0==strncmp(token[0],"SW", MAX_OPCODE_LENGTH)){
		}else if(0==strncmp(token[0],"LD", MAX_OPCODE_LENGTH)){
			instr[j] = (Instr*)malloc(sizeof(Instr));
			instr[j]->updateRD = 0;
			instr[j]->updateWR = 0;
			instr[j]->done = 0;
			instr[j]->issued = 0;
			instr[j]->fu = &intUnitCount;
			instr[j]->rd = freg[getFloatRegIndex(token[1])];
			instr[j]->rs1 = ireg[getIntRegIndexAd(token[2])];
			instr[j]->rs2 = NULL;
			instr[j]->counterEX = -1;
			instr[j]->clockEX = intClk;
			instr[j]->waitIndex = -1;
			instr[j]->issue = -1;
			instr[j]->read = -1;
			instr[j]->ex = -1;
			instr[j]->wb = -1;
			strcpy((instr[j]->name),temp);
			printf("loaded %s\n", instr[j]->name);
			j++;
		}else if(0==strncmp(token[0],"SD", MAX_OPCODE_LENGTH)){
		}else if(0==strncmp(token[0],"ADD", MAX_OPCODE_LENGTH)){
		}else if(0==strncmp(token[0],"ADDI", MAX_OPCODE_LENGTH)){
		}else if(0==strncmp(token[0],"SUB", MAX_OPCODE_LENGTH)){
		}else if(0==strncmp(token[0],"SUBI", MAX_OPCODE_LENGTH)){
		}else if(0==strncmp(token[0],"BNEZ", MAX_OPCODE_LENGTH)){
		}else if(0==strncmp(token[0],"ADDD", MAX_OPCODE_LENGTH)){
			instr[j] = (Instr*)malloc(sizeof(Instr));
			instr[j]->updateRD = 0;
			instr[j]->updateWR = 0;
			instr[j]->done = 0;
			instr[j]->issued = 0;
			instr[j]->fu = &addUnitCount;
			instr[j]->rd = freg[getFloatRegIndex(token[1])];
			instr[j]->rs1 = freg[getFloatRegIndex(token[2])];
			instr[j]->rs2 = freg[getFloatRegIndex(token[3])];
			instr[j]->counterEX = -1;
			instr[j]->clockEX = addClk;
			instr[j]->waitIndex = -1;
			instr[j]->issue = -1;
			instr[j]->read = -1;
			instr[j]->ex = -1;
			instr[j]->wb = -1;
			strcpy(instr[j]->name,temp);
			printf("loaded %s\n", instr[j]->name);
			j++;
		}else if(0==strncmp(token[0],"SUBD", MAX_OPCODE_LENGTH)){
			instr[j] = (Instr*)malloc(sizeof(Instr));
			instr[j]->updateRD = 0;
			instr[j]->updateWR = 0;
			instr[j]->done = 0;
			instr[j]->issued = 0;
			instr[j]->fu = &addUnitCount;
			instr[j]->rd = freg[getFloatRegIndex(token[1])];
			instr[j]->rs1 = freg[getFloatRegIndex(token[2])];
			instr[j]->rs2 = freg[getFloatRegIndex(token[3])];
			instr[j]->counterEX = -1;
			instr[j]->clockEX = addClk;
			instr[j]->waitIndex = -1;
			instr[j]->issue = -1;
			instr[j]->read = -1;
			instr[j]->ex = -1;
			instr[j]->wb = -1;
			strcpy(instr[j]->name,temp);
			printf("loaded %s\n", instr[j]->name);
			j++;
		}else if(0==strncmp(token[0],"MULTD", MAX_OPCODE_LENGTH)){
			instr[j] = (Instr*)malloc(sizeof(Instr));
			instr[j]->updateRD = 0;
			instr[j]->updateWR = 0;
			instr[j]->done = 0;
			instr[j]->issued = 0;
			instr[j]->fu = &multUnitCount;
			instr[j]->rd = freg[getFloatRegIndex(token[1])];
			instr[j]->rs1 = freg[getFloatRegIndex(token[2])];
			instr[j]->rs2 = freg[getFloatRegIndex(token[3])];
			instr[j]->counterEX = -1;
			instr[j]->clockEX = multClk;
			instr[j]->waitIndex = -1;
			instr[j]->issue = -1;
			instr[j]->read = -1;
			instr[j]->ex = -1;
			instr[j]->wb = -1;
			strcpy(instr[j]->name,temp);
			printf("loaded %s\n", instr[j]->name);
			j++;
		}else if(0==strncmp(token[0],"DIVD", MAX_OPCODE_LENGTH)){
			instr[j] = (Instr*)malloc(sizeof(Instr));
			instr[j]->updateRD = 0;
			instr[j]->updateWR = 0;
			instr[j]->done = 0;
			instr[j]->issued = 0;
			instr[j]->fu = &divUnitCount;
			instr[j]->rd = freg[getFloatRegIndex(token[1])];
			instr[j]->rs1 = freg[getFloatRegIndex(token[2])];
			instr[j]->rs2 = freg[getFloatRegIndex(token[3])];
			instr[j]->counterEX = -1;
			instr[j]->clockEX = divClk;
			instr[j]->waitIndex = -1;
			instr[j]->issue = -1;
			instr[j]->read = -1;
			instr[j]->ex = -1;
			instr[j]->wb = -1;
			strcpy(instr[j]->name,temp);
			printf("loaded %s\n", instr[j]->name);
			j++;
		}

		// Reset i;
		i=0;
	}
}

/*
 * The scoreboard processing function
 */
void scoreboard(){

	printf("==================================\n Executing ... ... \n==================================\n");

	// initialize the global variables: pc and clock. 
	// local variables: 
	// 	newIssue 
	// 		- if there is new instruction issued in the current clock cycle 
	// 	finished 
	// 		- if the program finished 
	pc = 0;
	clock = 0;
	int newIssue = 0; 
	int finished = 0;

#ifdef DEBUG
	printf("clock: %d	PC: %d\n", clock, pc);
	printInstrStatus();
	printIntReg();
	printFloatReg();
	printUnitStatus();
#endif /* DEBUG */

	// processing the program until all the instructions are done.
	// Or clock reaches the maximum clock cycles.
	while(!finished && clock < MAX_CLK_CYCLES){

		// Increase the clock and reset the newIssue and finished
		clock++;
		newIssue = 0;
		finished = 1;

		// check if it's issued
		if(NULL!=instr[pc] && !instr[pc]->issued){
			// check functional unit and rd availability
			if(*(instr[pc]->fu)>0 && (-1 == instr[pc]->rd->write)){
				instr[pc]->issue = clock;
				instr[pc]->issued = 1;
				*(instr[pc]->fu) = *(instr[pc]->fu)-1;
				instr[pc]->rd->write = pc;
				if(instr[pc]->rs1->read<pc){
					instr[pc]->rs1->read = pc;
				}
				if (NULL != instr[pc]->rs2 && instr[pc]->rs2->read<pc){
					instr[pc]->rs2->read = pc;
				}
#ifdef DEBUG
				printf("Instr %d issued. \n", pc);
#endif /* DEBUG */
				pc++;
				newIssue = 1;
			}
#ifdef DEBUG
			else{
				printf("Instr %d failed to issue. \n", pc);
			}
#endif /* DEBUG */
		}

		// processes all the instructions
		for(i=0;i<(pc-newIssue);i++){

#ifdef DEBUG
			printf("Instr %d entered processing queue\n", i);
#endif /* DEBUG */
			// check done
			if(!instr[i]->done){
				// check counterEX and wait index
				if(-1 == instr[i]->counterEX){
					if(NULL == instr[i]->rs2){
						if(instr[i]->rs1->write>i || (-1==instr[i]->rs1->write)){
							instr[i]->counterEX = 0;
							instr[i]->read = clock;
							instr[i]->updateRD = pc;
						}
					}else
						// check rs1 and rs2 availability
						if(((instr[i]->rs1->write>i) || (-1==instr[i]->rs1->write)) && ((instr[i]->rs2->write>i) || (-1==instr[i]->rs2->write))){
							instr[i]->counterEX = 0;
							instr[i]->read = clock;
							instr[i]->updateRD = pc;
						}
				} else{
					if(instr[i]->counterEX+1 < instr[i]->clockEX){
						instr[i]->counterEX++;
					}else if(instr[i]->counterEX+1 == instr[i]->clockEX){
						instr[i]->counterEX++;
						instr[i]->ex = clock;
					}else{
						if(instr[i]->rd->read>i || (-1 == instr[i]->rd->read)){
							instr[i]->updateWR = 1;
							instr[i]->done = 1;
							instr[i]->wb = clock;
						}
					}
				}
			}
		}

		// update the register status tables
		for(i=0;i<pc;i++){
			if(instr[i]->updateWR){
				instr[i]->rd->write = -1;
				*(instr[i]->fu)=*(instr[i]->fu)+1;
				instr[i]->updateWR = 0;
			}
			if(instr[i]->updateRD){
				if(i==instr[i]->rs1->read){
					instr[i]->rs1->read = -1;
				}
				if(NULL != instr[i]->rs2 && (i==instr[i]->rs2->read)){
					instr[i]->rs2->read = -1;
				}
				instr[i]->updateRD = 0;
			}
		}

		// check if all the instructions are finished
		i = 0;
		while(finished && NULL != instr[i]){
			if(!instr[i]->done){
				finished = 0;
			}
			i++;
		}

#ifdef DEBUG
		printf("clock: %d	PC: %d\n", clock, pc);
		printInstrStatus();
		printIntReg();
		printFloatReg();
		printUnitStatus();
#endif /* DEBUG */	

	}
	printf("==================================\n Execution is DONE! \n==================================\n");

}

/*
 * The main()
 */
int main(int argc, char* argv[]){

	char* prog_name;

	if(argc != 2){
		printf("===================================\n| usage: scoreboard [Program file] |\n===================================\n");
		exit(-1);
	} else {
		prog_name = argv[1];
	}

	printf("Initializing the Scoreboard Parameters ... ...\n");
	init();
	printf("Finished initialization \n");
	loadProgram(prog_name);

#ifdef DEBUG
	printInstrStatus();
#endif /* DEBUG */

	scoreboard();
	printScoreboardResult();

	return 0;
}

