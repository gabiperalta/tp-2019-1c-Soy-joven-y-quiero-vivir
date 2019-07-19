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
	uint32_t timestamp;
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


//ESTRUCTURA PARA LA COLA AUXILIAR DEL LRU
typedef struct{
	char *path;
	int numeroPagina;
	int modificado;
}t_auxSegmento;

t_segmento* crearSegmento(char *path);
t_pagina* crearPagina(int numeroPagina,int modificado,t_registro registro);
t_segmento* buscarSegmento(t_list* lista,char *path);
t_pagina* buscarPagina(t_list* lista,uint16_t key);
void* guardarRegistro(t_registro registro);
void actualizarRegistro(t_pagina* pagina,t_registro registro);
char* obtenerValue(void* direccion);
uint32_t obtenerTimestamp(void* direccion);
uint32_t getCurrentTime();
t_list quePasarEnElJournal(t_list* tabla_segmentos);
int cuantasNoModif(t_list* tabla_segmentos);

t_auxSegmento* crearAuxSegmento(char* path, t_pagina* pagina);
void agregarEnListaLRU(t_list* auxLRU,t_segmento* segment, t_pagina* page);
void destructorDeSegmentoAUX(t_auxSegmento auxSeg);
void eliminarSegmentoLRU(t_list* auxLRU, t_segmento* segment);
t_auxSegmento* cualTengoQueSacar(t_list* auxLRU);
void quitarLuegoDeDrop(t_list* auxLRU,t_segmento *segment);

void destructorDePagina(t_pagina* pagina);
void destructorDeSegmento(t_segmento segment);
void eliminarSegmento(t_list* lista, t_segmento* segment);
int saberCantidadDePaginasEliminadas(t_segmento* segment);

void vaciarMemoria(t_segmento* segment, t_list* auxLRU);

#endif /* SEGMENTACIONPAGINADA_H_ */
