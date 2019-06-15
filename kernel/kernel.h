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
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>
#include "../biblioteca/biblioteca_sockets.h"
#include "../biblioteca/biblioteca.h"
#include "funcionesKernel.h"

#define MULTIPROCESAMIENTO 1

t_queue* queue_nuevo;
t_queue* queue_listo;

sem_t semaforoNuevo;
sem_t mutexNuevo;
sem_t semaforoListo;
sem_t mutexListo;
sem_t semaforoExecLibre;
sem_t semaforoExecOcupado;

#endif /* KERNEL_H_ */
