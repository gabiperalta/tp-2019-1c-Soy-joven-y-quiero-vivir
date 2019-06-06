/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "filesystem.h"
#include "bibliotecaLFS.h"
#include <commons/string.h>
#include <time.h>




int gestionarFuncionFilesystem(char* solicitud) {
	char** preSpliteado = string_split(solicitud, '"')
	char** spliteado = string_split(preSpliteado[0], ' ');


	if(!strcmp(spliteado[0], "SELECT")) {

		char* valor = selectLFS(spliteado[1], spliteado[2]);

		printf("El valor de la key %s mas nueva es %s\n", spliteado[2], valor);

	}

	else if(!strcmp(spliteado[0], "INSERT")) {

		if(cantidadElementosCharAsteriscoAsterisco(preSpliteado) == 3){

			insertLFS(spliteado[1], spliteado[2], preSpliteado[1], preSpliteado[2]);

		}
		else{

			insertLFS(spliteado[1], spliteado[2], preSpliteado[1], "USE_TIMESTAMP");

		}
	}

	else if(!strcmp(spliteado[0], "CREATE")) {

		createLFS(spliteado[1], spliteado[2], spliteado[3], spliteado[4]);

	}

	else if(!strcmp(spliteado[0], "DESCRIBE")) {

		if(cantidadElementosCharAsteriscoAsterisco(spliteado) > 1){

			describeLSF(spliteado[1]);

		}
		else{

			describeLSF("DEFAULT");

		}
	}

	else if(!strcmp(spliteado[0], "DROP")) {

		dropLSF(spliteado[1]);

	}
	else if(!strcmp(spliteado[0], "DUMP")) {

		dump();

	}

	else {

		printf("La funcion no es correcta\n");

	}

	free(spliteado);
	liberarCharAsteriscoAsterisco(preSpliteado);
	liberarCharAsteriscoAsterisco(spliteado);

	return 0;
}

int main() {

	char * linea;
	inicializarMemtable();
	//inicializarServidor();
	/*pthread_t dumpThread;
	//char *arg = "hilo1: ";
	pthread_create(&dumpThread, NULL, (void*)dump, NULL);
	pthread_detach(dumpThread);*/
	while(1) {
		linea = readline(">"); //----- CREATE TABLA1 SC 3 60000 ----- SELECT TABLA1 4 -----	INSERT TABLA1 4 "HOLAPIPI" ----- DROP TABLA2
		if (!linea) {		   // ---------------------------------------------------------------------------------------CREATE TABLA2 SC 4 60000
			break;
		}
		gestionarFuncionFilesystem(linea);
		free(linea);
	}

	return 0;
}
