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
#include "../biblioteca/gossiping.h"
#include <unistd.h>
#include <semaphore.h>

#define MAX_VALUE 30
#define PATH_CONFIG "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/memoria/memoria.config"
#define PATH_LOG "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/memoria/memoria.log"

void* memoria;
int tamano_registro;
t_list* tabla_segmentos;
int puerto;
pthread_mutex_t mutex;
t_config* archivo_config;
t_log* logMemoria; //copie de fede
int tamano_memoria;
int puerto_escucha_memoria;
t_list* tabla_gossiping;

sem_t mutexEscrituraMemoria;
sem_t mutexEscrituraMemoria;

#endif /* MEMORIA_H_ */
