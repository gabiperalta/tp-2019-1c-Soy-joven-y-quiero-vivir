/*
 * kernel.h
 *
 *  Created on: 9 jun. 2019
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

//Estructura de instrucción

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>
#include "../biblioteca/biblioteca_sockets.h"
#include "../biblioteca/biblioteca.h"
#include "funcionesKernel.h"

//#define MULTIPROCESAMIENTO 1
//#define QUANTUM 3
#define PATH_CONFIG "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/kernel/kernel.config"
#define PATH_LOG "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/kernel/kernel.log"

t_queue* queue_nuevo;
t_queue* queue_listo;

sem_t semaforoNuevo;
sem_t mutexNuevo;
sem_t semaforoListo;
sem_t mutexListo;
sem_t semaforoExecLibre;

sem_t mutexCriterio;
sem_t mutexMetadata;

t_list* criterio_SC;
t_list* criterio_SHC;	// no se usa por el recorte
t_list* criterio_EC;

t_list* metadata_tablas;
t_list* tabla_gossiping;

t_config* archivo_config;
int multiprocesamiento;
int quantum;
char* ip_memoria;
int puerto_memoria;

t_log* archivo_log;

#endif /* KERNEL_H_ */
