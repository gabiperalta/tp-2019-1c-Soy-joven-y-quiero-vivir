/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "kernel.h"



int gestionarFuncionKernel(char* solicitud) {
	char** spliteado = string_split(solicitud, " ");


	if(!strcmp(spliteado[0], "SELECT")) {
		printf("---select\n");
		int servidor = conectarseA(IP_LOCAL, PUERTO_ESCUCHA_MEM);
		enviarMensaje("Hola!", servidor);
		close(servidor);
//		enviarMensaje(spliteado[1], PUERTO_ESCUCHA_MEM); //IMPLEMENTACIÓN DE ENVIAR MENSAJE DE PRUEBA
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

	else if(!strcmp(spliteado[0], "JOURNAL")) {
		printf("---journal\n");
	}

	else if(!strcmp(spliteado[0], "ADD")) {
		printf("---add\n");
	}

	else if(!strcmp(spliteado[0], "RUN")) {
		printf("---run\n");
	}

	else if(!strcmp(spliteado[0], "METRICS")) {
		printf("---metrics\n");
	}

	else {
		printf("La función no es correcta\n");
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

