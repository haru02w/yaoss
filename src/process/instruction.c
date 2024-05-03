#include "instruction.h"

instruction_t *inst_read(char *string) {
	int code = 0;
	if(string == NULL){
		printf("string Vazia\n");
		return NULL;
	}
    int i;
	for (i = 0; string[i] != ' ' && string[i] != '(';i++){
		code = (code + string[i]) % 10;
	}
	instruction_t *inst =(instruction_t*) malloc(sizeof(instruction_t));
	switch(code){
		case 0:{ //P()
			inst->op = P;
			//TODO: tratamento semaforo
			printf("semaforo n implemntado\n");
			break;
		}
		case 1: inst->op = EXEC; break;
		case 2: inst->op = READ; break;
		case 3: puts("erro1"); break;
		case 4: puts("erro2"); break;
		case 5: inst->op = WRITE; break;
		case 6:{ //V()
			inst->op = V;
			//TODO: tratamento semaforo
			printf("semaforo n implemntado\n");
			break;
		}
		case 7: inst->op = PRINT; break;
	}
	inst->value = atoi(&(string[i]));

	return inst;
}
