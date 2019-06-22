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

	t_registro registro;
	int tamano_registro = sizeof(registro.key) + sizeof(registro.timestamp) + MAX_VALUE;

	memoria = malloc(tamano_memoria);
	tabla_segmentos = list_create();
	memset(memoria,NULL,tamano_memoria); //inicializa la memoria en NULL

	prueba(memoria,tabla_segmentos);

	puerto = escuchar(puerto_escucha_memoria); //antes estaba PUERTO_ESCUCHA_MEM

	// Inicializacion de semaforos
	sem_init(&mutexEscrituraMemoria,NULL,1);

	// Creacion de hilos
	pthread_t servidorKernel;
	pthread_t hiloConsola;
	pthread_create(&servidorKernel,NULL,(void*)conexionKernel,NULL);
	pthread_detach(servidorKernel);
	pthread_create(&hiloConsola,NULL,(void*)consola,NULL);
	pthread_join(hiloConsola,NULL);

	return 0;
}
