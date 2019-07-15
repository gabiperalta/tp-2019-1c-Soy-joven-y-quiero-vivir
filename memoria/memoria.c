/*
 * memoria.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "memoria.h"


int main(){

	archivo_config = config_create(PATH_CONFIG);
	tamano_memoria = obtenerTamanioMemo();
	puerto_escucha_memoria = obtenerPuertoConfig();

	int conectado = 0;
	t_request request_ingresada;
	t_list* auxLRU= list_create();
	t_registro registro;
	tamano_registro = sizeof(registro.key) + sizeof(registro.timestamp) + MAX_VALUE;

	// Inicializacion
	inicializarLogMemo();
	tabla_segmentos = list_create();
	tabla_gossiping = list_create();
	memoria = malloc(tamano_memoria);
	memset(memoria,NULL,tamano_memoria); //inicializa la memoria en NULL

	// se carga un registro solo para prueba
	prueba(memoria,tabla_segmentos);

	puerto = escuchar(puerto_escucha_memoria); //antes estaba PUERTO_ESCUCHA_MEM

	// Inicializacion de semaforos
	sem_init(&mutexEscrituraMemoria,NULL,1);

	// Creacion de hilos
	pthread_t hiloServidor;
	pthread_t hiloConsola;
	pthread_t hiloGossiping;
	pthread_create(&hiloServidor,NULL,(void*)servidor,NULL);
	pthread_detach(hiloServidor);
	pthread_create(&hiloGossiping,NULL,(void*)procesoGossiping,NULL);
	pthread_detach(hiloGossiping);

	pthread_create(&hiloConsola,NULL,(void*)consola,NULL);
	pthread_join(hiloConsola,NULL);

	return 0;
}
