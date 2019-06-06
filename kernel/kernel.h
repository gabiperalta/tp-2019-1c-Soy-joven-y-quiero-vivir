/*
 * filesystem.h
 *
 *  Created on: 12 abr. 2019
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

//Estructura de instrucción

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include "../biblioteca/biblioteca_sockets.h"






/*TAMANIODEARCHIVO
 * Devuelve el tamaño de un archivo.
 */
int32_t tamanioDeArchivo(FILE* f) {
	int32_t previo = ftell(f);
	fseek(f, 0, SEEK_END);
	int32_t tamanio = ftell(f);
	fseek(f, previo, SEEK_SET);
	return tamanio;
}

int8_t cantidadElementosCharAsteriscoAsterisco(char** array) {
	int8_t  size;
	for (size = 0; array[size] != NULL; size++);

	return size;
}

/* LASINTAXISESCORRECTA
 * Devuelve TRUE en el caso que la cantidad de elementos sean correctos
 * Caso contrario devuelve FALSE
 */
bool laSintaxisEsCorrecta(char** funcion, int8_t cantidadDeElementos) {
	if(!strcmp(funcion[0],"SELECT") && cantidadDeElementos == 3)
		return true;

	if(!strcmp(funcion[0], "CREATE") && cantidadDeElementos == 5)
			return true;

	if(!strcmp(funcion[0], "DESCRIBE") && cantidadDeElementos == 2)
			return true;

	if(!strcmp(funcion[0], "DROP") && cantidadDeElementos == 2)
			return true;

	if(!strcmp(funcion[0], "INSERT") && cantidadDeElementos == 5) //CON TIMESTAMP
			return true;

	if(!strcmp(funcion[0], "INSERT") && cantidadDeElementos == 4) //SIN TIMESTAMP
				return true;

	if(!strcmp(funcion[0], "JOURNAL") && cantidadDeElementos == 1)
			return true;
//
//	if(funcion[0] == "ADD" && strlen(funcion) == 4)
//			return true;

	return false;
}


/*	PARSEARLQL
 * Recibe un archivo y un array de punteros, aloca cada fila del txt
 * en una posición del array y retorna la cantidad de elementos que tiene
 * el archivo.
 */

//FUNCIONA PERO SI AL FINAL DEL TXT HAY UN "ENTER" DEVUELVE BASURA
int16_t parsearLQL(FILE* f, char** buffer) {
	int32_t tamanioArchivo = tamanioDeArchivo(f);
	int16_t i = 0;
	while(!feof(f)){
		char* auxiliar = malloc(tamanioArchivo);
		buffer[i] = malloc(30);

		fgets(auxiliar, tamanioArchivo, f);
		if(strlen(auxiliar) > 0){
			strcpy(buffer[i], auxiliar);
			i++;
		}

		free(auxiliar);
	}

	return i;
}




/*PARSEAR POR ESPACIO
 *
 */
char** parsear(char** cosa, char* cadena) {
	cosa = string_split(cadena, " ");
	return cosa;
}





#endif /* KERNEL_H_ */
