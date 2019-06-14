/*
 * kernel.c
 *
 *  Created on: 9 jun. 2019
 *      Author: utnso
 */

#include "kernel.h"

// SELECT TABLA1 16

int main() {

	char * linea;
	t_request request_ingresada;

	queue_nuevo = queue_create();
	queue_listo = queue_create();

	sem_init(&semaforoNuevo,NULL,0);
	sem_init(&mutexNuevo,NULL,1);
	sem_init(&semaforoListo,NULL,0);
	sem_init(&mutexListo,NULL,1);
	sem_init(&semaforoExecLibre,NULL,3); //multiprocesamiento = 3
	sem_init(&semaforoExecOcupado,NULL,3);

	pthread_t hiloAtenderNuevos;
	pthread_t hiloAtenderListos;

	pthread_create(&hiloAtenderNuevos,NULL,(void*)atenderNuevos,NULL);
	pthread_detach(&hiloAtenderNuevos);
	pthread_create(&hiloAtenderListos,NULL,(void*)atenderListos,NULL);
	pthread_detach(&hiloAtenderListos);

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

