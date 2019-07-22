/*
 * memoria.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "memoria.h"


int main(){

	pthread_mutex_lock(&mutexMemoriaLlena);
	flagFullEnviado = 0;

	archivo_config = config_create(PATH_CONFIG);
	//tamano_memoria = obtenerTamanioMemo();
	tamano_memoria = 144; // SOLO PARA PRUEBA, capacidad para 4 paginas
	puerto_escucha_memoria = obtenerPuertoConfig();
	ip_fs = obtenerIP_FS();
	puerto_fs = obtenerPuertoFS();

	int conectado = 0;
	t_request request_ingresada;
	t_registro registro;
	tamano_registro = sizeof(registro.key) + sizeof(registro.timestamp) + MAX_VALUE; // 36

	// Inicializacion
	inicializarLogMemo();
	log_info(logMemoria,"====================== INICIO ======================");
	tabla_segmentos = list_create();
	tabla_gossiping = list_create();
	lista_LRU= list_create();
	memoria = malloc(tamano_memoria);
	memset(memoria,NULL,tamano_memoria); //inicializa la memoria en NULL
	agregarMemoriaGossiping(); // se agrega la propia memoria a la tabla

	cantTotalPaginas = tamano_memoria / tamano_registro;
	cantPaginasLibres = cantTotalPaginas;

	// se carga un registro solo para prueba
	prueba(memoria,tabla_segmentos);

	puerto = escuchar(puerto_escucha_memoria); //antes estaba PUERTO_ESCUCHA_MEM

	// Inicializacion de semaforos
	sem_init(&mutexAccesoMemoria,NULL,1);

	// Creacion de hilos
	pthread_t hiloServidor;
	pthread_t hiloConsola;
	pthread_t hiloGossiping;
	//pthread_t hiloJournal;
	pthread_create(&hiloServidor,NULL,(void*)servidor,NULL);
	pthread_detach(hiloServidor);
	pthread_create(&hiloGossiping,NULL,(void*)procesoGossiping,NULL);
	pthread_detach(hiloGossiping);
	//pthread_create(&hiloJournal,NULL,(void*)procesoJournal(),NULL);
	//pthread_detach(hiloJournal);

	pthread_create(&hiloConsola,NULL,(void*)consola,NULL);
	pthread_join(hiloConsola,NULL);

	return 0;
}
