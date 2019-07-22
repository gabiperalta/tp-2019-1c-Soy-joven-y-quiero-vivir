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
}t_segmento;

typedef struct{
	int numeroPagina;
	void* direccion;
	int modificado;
}t_pagina;

typedef struct{
	char *path;
	int numeroPagina;
	int modificado;
}t_registro_LRU;

//ESTRUCTURA PARA PASAR EN EL JOURNAL
typedef struct{
	char* path;
	uint32_t timestamp;
	char* value;
}t_registroJOURNAL;

t_segmento* crearSegmento(char *path);
t_pagina* crearPagina(int numeroPagina,int modificado,t_registro registro);
t_registro* crearRegistro(t_response respuesta, uint16_t keyNuevo);
t_segmento* buscarSegmento(t_list* lista,char *path);
t_pagina* buscarPagina(t_list* lista,uint16_t key);
t_pagina* buscarPaginaPorNumero(t_list* lista,int numeroPagina);
void* guardarRegistro(t_registro registro);
void actualizarRegistro(t_pagina* pagina,t_registro registro);
char* obtenerValue(void* direccion);
uint16_t obtenerKey(void* direccion);
uint32_t obtenerTimestamp(void* direccion);
int obtenerIndicePagina(t_list* lista);
uint32_t getCurrentTime();

t_registroJOURNAL* crearRegistroJOURNAL(char* path, t_pagina* pagina);
void filtrarModificados(t_list* listaOriginal, t_list* tabla_segmento);
t_list* quePasarEnElJournal(t_list* tabla_segmentos);
t_pagina* buscarPaginaModificadaONO(t_list* lista, int flag);


t_registro_LRU* crearRegistroLRU(char* path, t_pagina* pagina);
void agregarEnListaLRU(char* path, t_pagina* page);
void destructorDeSegmentoAUX(t_registro_LRU* auxSeg);
void eliminarRegistroLRU(t_registro_LRU* registro_LRU);
int liberarMemoriaLRU();

void dropSegmento(t_segmento* segment);

void eliminarSegmento(t_segmento* segment);
void eliminarPagina(t_pagina* pagina);
int cantidadDePaginasEliminadas(t_segmento* segment);

void vaciarMemoria();
//void vaciarMemoria(t_segmento* segment, t_list* auxLRU);

#endif /* SEGMENTACIONPAGINADA_H_ */
