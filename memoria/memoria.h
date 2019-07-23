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

t_config* archivo_config;
t_log* logMemoria;

int puerto;
int puerto_escucha_memoria;

t_list* tabla_segmentos;
t_list* tabla_gossiping;
t_list* lista_LRU;

pthread_mutex_t mutex;
pthread_mutex_t mutexMemoriaLlena;
pthread_mutex_t mutexTablaGossiping;

sem_t mutexAccesoMemoria;

int tamano_registro;
int tamano_memoria;
int cantPaginasLibres;
int cantTotalPaginas;
int flagFullEnviado;

char* ip_fs;
int puerto_fs;

#endif /* MEMORIA_H_ */
