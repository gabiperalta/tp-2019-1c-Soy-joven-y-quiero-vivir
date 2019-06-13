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
#include "../biblioteca/biblioteca_sockets.h"
#include "../biblioteca/biblioteca.h"


t_queue* queue_nuevo = queue_create();
t_queue* queue_listo = queue_create();

#endif /* KERNEL_H_ */
