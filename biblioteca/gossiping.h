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

typedef struct{

	int id;
	uint8_t tam_ip;
	char* ip;
	int puerto;

}t_memoria;

void iniciarGossiping(int servidor);
void enviarTablaGossiping(int cliente, t_list* tabla_gossiping);
t_list* recibirTablaGossiping(int servidor);
t_list* obtenerUnion(t_list* lista1, t_list* lista2);
t_memoria* buscarMemoria(t_list* lista,int id);
t_memoria* buscarMemoriaPorIP(t_list* lista,char* ip);
t_memoria* buscarMemoriaPorPuerto(t_list* lista,int puerto);
void eliminarMemoria(t_list* lista, int id);

#endif /* GOSSIPING_H_ */

