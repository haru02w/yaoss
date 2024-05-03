#include "process.h"


pdata_t *program_init(char *path){
	if(!path){
		printf("erro, sem nome de arquivo\n");
		return NULL;
	}
	pdata_t *process = malloc(sizeof(pdata_t));
	FILE *fp = fopen(path, "r");
	if(!fp){
		printf("arquivo não encontrado\n");
		free(process);
		return NULL;
	}
	char buffer[50];
	fgets(buffer, sizeof(buffer), fp);
	strcpy(process->nome, buffer);
	fgets(buffer, sizeof(buffer), fp);
	process->seg = atoi(buffer);
	fgets(buffer, sizeof(buffer), fp);
	process->priority = atoi(buffer);
	fgets(buffer, sizeof(buffer), fp);
	process->seg_size = atoi(buffer);
	//TODO: ler lista de semaforos



}
