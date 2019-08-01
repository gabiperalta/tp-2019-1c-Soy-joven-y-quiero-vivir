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

void agregarMemoriaGossiping();

void consola();
void solicitudConsola(char* linea);
void prueba(void* memoria,t_list* tabla_segmentos);
void procesoGossiping();
void procesoJournal();
t_response procesarRequest(t_request request);
void servidor();
void atenderRequest(void* cliente);
t_response solicitarFS(t_request request);
int handshakeFS();

char* obtenerIP();
int obtenerPuertoConfig();
int obtenerTamanioMemo();
void modificarRetardos(char* tipoRetardo,int valorNuevo);
int obtenerRetardo(char* tipoRetardo);
char* obtenerIP_FS();
int obtenerPuertoFS();
char** obtenerIP_SEEDS();
char** obtenerPUERTO_SEEDS();
int obtenerIdMemoria();

void inicializarArchivoConfig();

void liberarRecursos();
int cantidadDePaginas(int tamanioMemo);

void inicializarLogMemo();
void enviarListaJournal(int cliente, t_list* listaJournal);
void eliminarListaJournal(t_request* request);
#endif /* FUNCIONES_H_ */
