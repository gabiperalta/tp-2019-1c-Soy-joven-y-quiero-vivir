/*
 * kernel.c
 *
 *  Created on: 9 jun. 2019
 *      Author: utnso
 */

#include "kernel.h"


int main() {

	/*
	t_queue* scripts;
	scripts = queue_create();
	 */



	char * linea;
	t_request request_ingresada;

	system("clear");

	while(1) {
		linea = readline(">");
		if(linea)
			add_history(linea);
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}



		request_ingresada = gestionarSolicitud(linea);

		procesarRequest(request_ingresada.header,linea);

		free(linea);
	}

	return 0;
}

