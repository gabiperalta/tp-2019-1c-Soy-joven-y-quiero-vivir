/*
 * segmentacionPaginada.c
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#include "segmentacionPaginada.h"

static t_segmento *crearSegmento(char *path) {
    t_segmento* new = malloc(sizeof(t_segmento));
    new->path = strdup(path);
    new->tabla_pagina = list_create();
    return new;
}

static t_pagina *crearPagina(int numeroPagina,int modificado) {
    t_pagina* new = malloc(sizeof(t_pagina));
    new->numeroPagina = numeroPagina;
    new->modificado = modificado;
    return new;
}

t_segmento *buscarSegmento(t_list* lista,char *path) {
	int igualPath(t_segmento *p) {
		return string_equals_ignore_case(p->path, path);
	}

	return list_find(lista, (void*) igualPath);
}

void* guardarRegistro(void* memoria,t_registro registro){

	void * direccion;
	int posicion = 0;
	direccion = memcpy(memoria,&registro.timestamp,sizeof(registro.timestamp));
	posicion += sizeof(registro.timestamp);
	memcpy(&memoria[posicion],&registro.key,sizeof(registro.key));
	posicion += sizeof(registro.key);
	memcpy(&memoria[posicion],registro.value,strlen(registro.value) + 1);
	//posicion += VALUE;
	return direccion;
}
