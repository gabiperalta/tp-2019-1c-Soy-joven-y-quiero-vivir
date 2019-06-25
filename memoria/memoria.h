/*
 * memoria.h
 *
 *  Created on: 13 abr. 2019
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <commons/string.h>
#include "../biblioteca/biblioteca_sockets.h"
#include "segmentacionPaginada.h"
#include <unistd.h>
#include <semaphore.h>

#define MAX_VALUE 30
#define PATH_CONFIG "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/memoria/memoria.config"

typedef struct{

	int id;
	uint8_t tam_ip;
	char* ip;
	int puerto;

}t_memoria;

void* memoria;
t_list* tabla_segmentos;
int puerto;
pthread_mutex_t mutex;
t_config* archivo_config;
int tamano_memoria;
int puerto_escucha_memoria;
t_list* tabla_gossiping;

sem_t mutexEscrituraMemoria;

#endif /* MEMORIA_H_ */
