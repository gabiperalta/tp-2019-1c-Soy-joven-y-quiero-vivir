/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

// voy a hacer un commit de prueba.

#include "filesystem.h"
#include <commons/string.h>


int gestionarFuncionKernel(char* solicitud) {
	char** spliteado = string_split(solicitud, " ");

	printf("%s", spliteado[0]);


	if(!strcmp(spliteado[0], "SELECT")) {
		printf("---select\n");
	}

	else if(!strcmp(spliteado[0], "INSERT")) {
		printf("---insert\n");
	}

	else if(!strcmp(spliteado[0], "CREATE")) {
		printf("---create\n");
	}

	else if(!strcmp(spliteado[0], "DESCRIBE")) {
		printf("---describe\n");
	}

	else if(!strcmp(spliteado[0], "DROP")) {
		printf("---drop\n");
	}

	return 0;
}

int main() {
	char * linea;
	while(1) {
		linea = readline(">");
		if (!linea) {
			break;
		}
		gestionarFuncionKernel(linea);
		free(linea);
	}

	return 0;
}


