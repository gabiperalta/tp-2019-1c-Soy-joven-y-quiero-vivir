/*
 * filesystem.h
 *
 *  Created on: 12 abr. 2019
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_


#define JOURNAL "6"



//Estructura de instrucción

typedef struct {
	int numero;
	char* instruccion;
} BloqueInstrucciones;

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



/* JOURNAL
 * Le envia a memoría el comando para realizar el JOURNAL.
 */
void journal() {
	printf("---select\n");
	int16_t servidor = conectarseA(IP_LOCAL, PUERTO_ESCUCHA_MEM);
	enviarMensaje(JOURNAL, servidor);
	close(servidor);

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
