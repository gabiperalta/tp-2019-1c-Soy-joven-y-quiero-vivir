/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "kernel.h"


int main() {
	int conexionExitosa = 0;

	printf("Hola, soy el kernel\n");

	while(conexionExitosa == 0) {
			conexionExitosa = conectarseA("127.0.0.1", PUERTO_ESCUCHA_MEM);
			sleep(5);
		}
	printf("Me conecté");
	for(;;);
}

