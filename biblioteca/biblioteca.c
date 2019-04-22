/*
 * biblioteca.c
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */


#include <biblioteca.h>

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
