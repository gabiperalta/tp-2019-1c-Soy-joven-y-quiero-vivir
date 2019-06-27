/*
 * funciones.h
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/time.h>
#include <pthread.h>
#include <limits.h>

#include "../biblioteca/biblioteca_sockets.h"
#include "../biblioteca/biblioteca.h"
#include "segmentacionPaginada.h"
#include "memoria.h"
//#include "gossiping.h"

void consola();
void enviarRequestFileSystem(t_request request);
void prueba(void* memoria,t_list* tabla_segmentos);
void procesarRequest(t_request request);
void servidor();
void atenderRequest(void* cliente);
void enviarFS(t_request request);

int obtenerPuertoConfig();
int obtenerTamanioMemo();
void modificarRetardos(char* tipoRetardo,int valorNuevo);
int obtenerRetardo(char* tipoRetardo);
char* obtenerIP_FS();
int obtenerPuertoFS();
char** obtenerIP_SEEDS();
char** obtenerPUERTO_SEEDS();
int obtenerIdMemoria();


void liberarRecursos();
int cantidadDePaginas(int tamanioMemo);

//void agregarEnListaLRU(t_list* auxLRU,t_segmento segment, t_pagina page);
//t_auxSegmento cualTengoQueSacar(t_list* auxLRU);

#endif /* FUNCIONES_H_ */
