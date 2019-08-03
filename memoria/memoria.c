/*
 * memoria.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "memoria.h"


int main(){

	// Inicializacion
	inicializarArchivoConfig();

	if(!handshakeFS()){
		system("clear");
		printf("No se pudo obtener el tamano maximo del value\n");
		config_destroy(archivo_config);
		return 0;
	}

	inicializarLogMemo();
	log_info(logMemoria,"====================== MEMORIA %d ======================",numero_memoria);
	tabla_segmentos = list_create();
	tabla_gossiping = list_create();
	lista_LRU= list_create();
	memoria = malloc(tamano_memoria);
	memset(memoria,NULL,tamano_memoria); //inicializa la memoria en NULL
	agregarMemoriaGossiping(); // se agrega la propia memoria a la tabla

	pthread_mutex_lock(&mutexMemoriaLlena);
	flagFullEnviado = 0;

	puerto_escucha_memoria = obtenerPuertoConfig();

	int conectado = 0;
	t_request request_ingresada;
	t_registro registro;
	//tamano_registro = sizeof(registro.key) + sizeof(registro.timestamp) + MAX_VALUE; // 36
	tamano_registro = sizeof(registro.key) + sizeof(registro.timestamp) + max_value; // 36

	cantTotalPaginas = tamano_memoria / tamano_registro;
	cantPaginasLibres = cantTotalPaginas;

	//prueba(memoria,tabla_segmentos); // se carga un registro solo para prueba

	puerto = escuchar(puerto_escucha_memoria); //antes estaba PUERTO_ESCUCHA_MEM

	// Creacion de hilos
	pthread_t hiloServidor;
	pthread_t hiloConsola;
	pthread_t hiloGossiping;
	pthread_t hiloJournal;
	//pthread_t hiloInotify;
	pthread_create(&hiloServidor,NULL,(void*)servidor,NULL);
	pthread_detach(hiloServidor);
	pthread_create(&hiloGossiping,NULL,(void*)procesoGossiping,NULL);
	pthread_detach(hiloGossiping);
	pthread_create(&hiloJournal,NULL,(void*)procesoJournal,NULL);
	pthread_detach(hiloJournal);
	//pthread_create(&hiloInotify,NULL,(void*)actualizacionArchivoConfig,NULL);
	//pthread_detach(hiloInotify);

	pthread_create(&hiloConsola,NULL,(void*)consola,NULL);
	pthread_join(hiloConsola,NULL);

	return 0;
}
