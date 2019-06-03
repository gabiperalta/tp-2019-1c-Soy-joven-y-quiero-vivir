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
	char** spliteado = string_split(solicitud, " ");


	if(!strcmp(spliteado[0], "SELECT")) {

		char* valor = selectLFS(spliteado[1], spliteado[2]);

		printf("El valor de la key %s mas nueva es %s\n", spliteado[2], valor);

	}

	else if(!strcmp(spliteado[0], "INSERT")) {

		if(string_length(spliteado) == 4){

			insertLFS(spliteado[1], spliteado[2], spliteado[3], spliteado[4]);

		}
		else{

			insertLFS(spliteado[1], spliteado[2], spliteado[3], "USE_TIMESTAMP");

		}
	}

	else if(!strcmp(spliteado[0], "CREATE")) {

		createLFS(spliteado[1], spliteado[2], spliteado[3], spliteado[4]);

	}

	else if(!strcmp(spliteado[0], "DESCRIBE")) {

		if(string_length(spliteado) > 1){

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

	return 0;
}

int main() {

	char * linea;
	inicializarMemtable();
	inicializarServidor();
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
