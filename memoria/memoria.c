/*
 * memoria.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "funciones.h"

int main(){
	int iniciar;
	int puerto = escuchar(PUERTO_ESCUCHA_MEM); // PUERTO_ESCUCHA_MEM = 36263
	int conectado = 0;

	printf("------------ MEMORIA ----------------\n");
	printf("Empezar prueba? 1 = si : ");
	scanf ("%d", &iniciar);


	while(iniciar > 0){ // cancelar con CTRL + C
		conectado = aceptarConexion(puerto);
		recibirPaquete(conectado);
		close(conectado);

		iniciar--;
	}


	return 0;
}
