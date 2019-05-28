/*
 * segmentacionPaginada.h
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#ifndef SEGMENTACIONPAGINADA_H_
#define SEGMENTACIONPAGINADA_H_

#include <commons/collections/list.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "funciones.h"

typedef struct{
	int timestamp;
	uint16_t key;
	char* value;
}t_registro;

typedef struct {
    char *path;
    t_list* tabla_pagina;
} t_segmento;

typedef struct{
	int numeroPagina;
	void* direccion;
	int modificado;
} t_pagina;

t_segmento* crearSegmento(char *path);
//static t_pagina *crearPagina(int numeroPagina,int modificado);
t_pagina* crearPagina(int numeroPagina,int modificado,void* memoria,t_registro registro);
t_segmento* buscarSegmento(t_list* lista,char *path);
t_pagina* buscarPagina(t_list* lista,uint16_t key,void* memoria);
void* guardarRegistro(void* memoria,t_registro registro);
void actualizarRegistro(t_pagina* pagina,t_registro registro);
char* obtenerValue(void* direccion);
int obtenerTimestamp(void* direccion);
int getCurrentTime();

#endif /* SEGMENTACIONPAGINADA_H_ */
