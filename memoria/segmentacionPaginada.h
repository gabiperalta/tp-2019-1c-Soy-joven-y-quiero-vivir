/*
 * segmentacionPaginada.h
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#ifndef SEGMENTACIONPAGINADA_H_
#define SEGMENTACIONPAGINADA_H_

#include <commons/collections/list.h>

typedef struct{
	int timestamp;
	int key;
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

static t_segmento *crearSegmento(char *path);
static t_pagina *crearPagina(int numeroPagina,int modificado);
t_segmento *buscarSegmento(t_list* lista,char *path);
void* guardarRegistro(void* memoria,t_registro registro);

#endif /* SEGMENTACIONPAGINADA_H_ */
