/*
 * filesystem.h
 *
 *  Created on: 12 abr. 2019
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_


#define JOURNAL "6"



//Estructura de instrucción

typedef struct {
	int numero;
	char* instruccion;
} BloqueInstrucciones;

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
#include "../biblioteca/biblioteca_sockets.h"







#endif /* KERNEL_H_ */
