/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "kernel.h"

/*	PARSEARLQL
 * Recibe un .LQL y retorna un array que en la primera posición
 * contiene la cantidad de comandos que hay, y en el resto los comandos.
 */

int tamanioDeArchivo(FILE* f) {
	int32_t previo = ftell(f);
	fseek(f, 0, SEEK_END);
	int32_t tamanio = ftell(f);
	fseek(f, previo, SEEK_SET);
	return tamanio;
}

char** parsearLQL(FILE* f, char** retorno) {
	int8_t i = 1;
	char* auxiliar = malloc(500+1);

	while(!feof(f)){
		fgets(auxiliar, 500+1, f );
		if(strlen(auxiliar) > 1){
			strcpy(retorno[i], auxiliar);
			i++;
		}
	}
	strcpy(retorno[0], string_itoa(i));
	free(auxiliar);
	return retorno;
}


int gestionarFuncionKernel(char* solicitud) {
	char** spliteado = string_split(solicitud, " ");

	if(!strcmp(spliteado[0], "SELECT")) {
		printf("---select\n");
		// int servidor = conectarseA(ipDeLaMemoria, puertoDeLaMemoria);
		int16_t servidor = conectarseA(IP_LOCAL, PUERTO_ESCUCHA_MEM);
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
		FILE* archivo = fopen("programa.lql", "r");
		char** programaParseado = malloc(tamanioDeArchivo(archivo));
		parsearLQL(archivo, programaParseado);
		fclose(archivo);
		free(programaParseado);
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

