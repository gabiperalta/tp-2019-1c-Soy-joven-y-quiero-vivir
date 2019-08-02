/*
 * gossiping.h
 *
 *  Created on: 26 jun. 2019
 *      Author: utnso
 */

#ifndef GOSSIPING_H_
#define GOSSIPING_H_

#include <commons/collections/list.h>
#include "biblioteca_sockets.h"
#include "biblioteca.h"

#define FLAG_KERNEL		0
#define FLAG_MEMORIA	1

typedef struct{

	int id;
	uint8_t tam_ip;
	char* ip;
	int puerto;

}t_memoria;

void iniciarGossiping(int servidor,int iniciador);
void enviarTablaGossiping(int cliente, t_list* tabla_gossiping);
t_list* recibirTablaGossiping(int servidor);
void obtenerUnion(t_list* lista_original, t_list* lista_agregada);
t_memoria* buscarMemoria(t_list* lista,int id);
t_memoria* buscarMemoriaPorIP(t_list* lista,char* ip);
t_memoria* buscarMemoriaPorPuerto(t_list* lista,int puerto);
t_memoria* buscarMemoriaPorIPyPuerto(t_list* lista,char* ip,int puerto);
void eliminarMemoria(t_list* lista, int id);
void liberarMemoriaGossiping(t_memoria* memoria);
bool idMenor(t_memoria *p, t_memoria *q);

#endif /* GOSSIPING_H_ */

