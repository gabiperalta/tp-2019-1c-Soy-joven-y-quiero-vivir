/*
 * memoria.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "memoria.h"
#include <commons/string.h>


int gestionarFuncionKernel(char* solicitud) {
	char** spliteado = string_split(solicitud, " ");

	string_to_upper(spliteado[0]);


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

	else if(!strcmp(spliteado[0], "JOURNAL")) {
		printf("---journal\n");
	}

	else {
		printf("La función no es correcta\n");
	}

	return 0;
}

void consola() {
	char * linea;
	while(1) {
		linea = readline(">");
		if (!linea) {
			break;
		}
		gestionarFuncionKernel(linea);
		free(linea);
	}
}

int main() {

	int puertoLoco= escuchar(PUERTO_ESCUCHA_MEM);

	int conectado = aceptarConexion(puertoLoco);
	recibirMensaje(conectado);
	close(conectado);

	conectado = aceptarConexion(puertoLoco);
	recibirMensaje(conectado);
	close(conectado);


	conectado = aceptarConexion(puertoLoco);
	recibirMensaje(conectado);
	close(conectado);

//		aceptarConexion(puertoLoco);
//		printf("Se conectó\n");
//		send(puertoLoco, "Hola", 5, 0);
//		send(puertoLoco, ":)", 3, 0);
//		recibirMensaje(puertoLoco);
//		close(puertoLoco);
	return 0;
}







