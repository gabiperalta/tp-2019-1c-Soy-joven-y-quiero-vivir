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

char** parsearLQL(char* nombreArchivo) {
	int i = 1;
	char* auxiliar = malloc(500+1);
	char** retorno = malloc (1000+1);
	FILE* f = fopen("programa.lql", "rt+");


	while(!feof(f)){
		fgets(auxiliar, 500+1, f );
		if(strlen(auxiliar) > 1){
			strcpy(retorno[i], auxiliar);
			i++;
		}
	}

	strcpy(retorno[0], string_itoa(i));

	free(auxiliar);
	fclose(f);
	return retorno;
}


int gestionarFuncionKernel(char* solicitud) {
	char** spliteado = string_split(solicitud, " ");

	// Crea un puntero al archivo de configuracion.
		/*t_config* archivoDeConfiguracion = config_create("Config.bin");
		// Devuelve el valor entero de una key (en este caso, PUERTO)
		int puertoDeLaMemoria = config_get_int_value(archivoDeConfiguracion, "PUERTO");
		char* ipDeLaMemoria = config_get_string_value(archivoDeConfiguracion, "IP_MEMORIA");
		*/

	if(!strcmp(spliteado[0], "SELECT")) {
		printf("---select\n");
		// int servidor = conectarseA(ipDeLaMemoria, puertoDeLaMemoria);
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

