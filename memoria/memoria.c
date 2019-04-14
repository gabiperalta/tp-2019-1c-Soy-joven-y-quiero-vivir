/*
 * memoria.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "memoria.h"

int main() {
	int conexionExitosa = 0; //0= FALSO 1= VERDADERO

	printf("Hola, soy la memoria\n");
	int numero = escuchar(PUERTO_ESCUCHA_MEM);
	int cliente = aceptarConexion(numero);
	printf("Se ha producido una conexión en: %i", cliente);

	while(conexionExitosa == 0) {
		conexionExitosa = conectarseA("127.0.0.1", PUERTO_ESCUCHA_FS);
		sleep(5);
	}

	printf("Me conecté");

	for(;;);

	return 0;
}







