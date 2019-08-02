/*
 * kernel.c
 *
 *  Created on: 9 jun. 2019
 *      Author: utnso
 */

// SELECT MAMIFEROS 3104

#include "kernel.h"

// SELECT TABLA1 16
// RUN /home/utnso/Descargas/1C2019-Scripts-lql-checkpoint-master/animales.lql
// RUN /home/utnso/Descargas/1C2019-Scripts-lql-entrega-master/scripts/simple_sc.lql

int main() {
	archivo_log = log_create(PATH_LOG,"kernel",false,LOG_LEVEL_INFO);
	archivo_config = config_create(PATH_CONFIG);
	leerArchivoConfig();
	ultimaMemoriaCriterioEC = 0;

	char * linea;
	t_request request_ingresada;

	queue_nuevo = queue_create();
	queue_listo = queue_create();

	tabla_gossiping = list_create();
	metadata_tablas = list_create();
	criterio_EC = list_create();
	criterio_SC = list_create();

	sem_init(&semaforoNuevo,NULL,0);
	sem_init(&mutexNuevo,NULL,1);
	sem_init(&semaforoListo,NULL,0);
	sem_init(&mutexListo,NULL,1);
	sem_init(&semaforoExecLibre,NULL,multiprocesamiento); //multiprocesamiento = 3

	pthread_t hiloAtenderNuevos;
	pthread_t hiloAtenderListos;
	pthread_t hiloGossiping;
	//pthread_t hiloMetadata;

	pthread_create(&hiloAtenderNuevos,NULL,(void*)atenderNuevos,NULL);
	pthread_detach(hiloAtenderNuevos);
	pthread_create(&hiloAtenderListos,NULL,(void*)atenderListos,NULL);
	pthread_detach(hiloAtenderListos);
	pthread_create(&hiloGossiping,NULL,(void*)procesoGossiping,NULL);
	pthread_detach(hiloGossiping);
	//pthread_create(&hiloMetadata,NULL,(void*)actualizarMetadata,NULL);
	//pthread_detach(hiloMetadata);

	system("clear");

	log_info(archivo_log,"====================== KERNEL ======================");

	while(1) {
		linea = readline(">");
		if(linea)
			add_history(linea);
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}

		log_info(archivo_log,"Se ingreso algo");
		log_error(archivo_log,"Se produjo un error");

		request_ingresada = gestionarSolicitud(linea);
		procesarRequest(request_ingresada.header,linea);
		liberarMemoriaRequest(request_ingresada);

		free(linea);
	}

	log_destroy(archivo_log);
	config_destroy(archivo_config);
	return 0;
}

