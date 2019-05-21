/*
 * funciones.c
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#include "funciones.h"

int gestionarFuncionKernel(char* solicitud){
	char** spliteado = string_split(solicitud, " ");


	string_to_upper(spliteado[0]);


	if(!strcmp(spliteado[0], "SELECT")){
		printf("---select\n");
	}

	else if(!strcmp(spliteado[0], "INSERT")){
		printf("---insert\n");
	}

	else if(!strcmp(spliteado[0], "CREATE")){
		printf("---create\n");
	}

	else if(!strcmp(spliteado[0], "DESCRIBE")){
		printf("---describe\n");
	}

	else if(!strcmp(spliteado[0], "DROP")){
		printf("---drop\n");
	}

	else if(!strcmp(spliteado[0], "JOURNAL")){
		printf("---journal\n");
	}

	else{
		printf("La función no es correcta\n");
	}

	return 0;
}

void consola(){
	char * linea;
	while(1) {
		linea = readline(">");
		if (!linea){
			break;
		}
		gestionarFuncionKernel(linea);
		free(linea);
	}
}

void recibirPaquete(int socketCliente){

	int bytesRecibidos;
	request prueba;

	void* buffer = malloc(1000);

	bytesRecibidos = recv(socketCliente, buffer, sizeof(prueba.clave), 0);

	if(bytesRecibidos <= 0) {
		perror("Error al recibir mensaje");
	}

	memcpy(&prueba.clave,buffer,sizeof(prueba.clave));
	printf("Recibí la clave: %d \n", prueba.clave);

	bytesRecibidos = recv(socketCliente, buffer, sizeof(prueba.tam_nombre_tabla), 0);
	memcpy(&prueba.tam_nombre_tabla,buffer,sizeof(prueba.tam_nombre_tabla));
	printf("El tamano del nombre de la tabla es: %d \n", prueba.tam_nombre_tabla);

	bytesRecibidos = recv(socketCliente, buffer, prueba.tam_nombre_tabla, 0);
	prueba.nombre_tabla = malloc(prueba.tam_nombre_tabla);
	memcpy(prueba.nombre_tabla,buffer,prueba.tam_nombre_tabla);
	printf("El nombre de la tabla es: %s \n\n", prueba.nombre_tabla);

	free(prueba.nombre_tabla);
	free(buffer);

}
