/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "filesystem.h"


int main() {

	printf("Hola, soy el filesystem\n");
	int numero = escuchar(PUERTO_ESCUCHA_FS);
	int cliente = aceptarConexion(numero);
	printf("Se ha producido una conexión en: %i", cliente);
	return 0;
}

