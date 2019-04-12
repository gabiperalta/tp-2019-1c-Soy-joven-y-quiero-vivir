/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "filesystem.h"


int main() {
	printf("Hola, soy el filesystem\n");
	int numero = escuchar(8085);

	aceptarConexion(numero);
	printf("Se ha producido una conexión");
	return 0;
}

