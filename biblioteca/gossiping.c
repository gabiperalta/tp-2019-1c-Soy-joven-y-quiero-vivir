/*
 * gossiping.c
 *
 *  Created on: 26 jun. 2019
 *      Author: utnso
 */

#include "gossiping.h"

void iniciarGossiping(int servidor){
	void* buffer = malloc(sizeof(uint8_t));
	uint8_t header = GOSSIPING;
	memcpy(buffer,&header,sizeof(uint8_t));
	send(servidor,buffer,sizeof(uint8_t),0);
	free(buffer);
}

void enviarTablaGossiping(int cliente, t_list* tabla_gossiping){
	int posicion = 0;

	int cantidadMemorias;

	int tamano_buffer;
	void* buffer;

	cantidadMemorias = list_size(tabla_gossiping);

	tamano_buffer = sizeof(cantidadMemorias);

	for(int i=0; i<cantidadMemorias; i++){
		t_memoria* mem_gossiping = list_get(tabla_gossiping,i);
		tamano_buffer += sizeof(mem_gossiping->id) + sizeof(mem_gossiping->tam_ip) + mem_gossiping->tam_ip + sizeof(mem_gossiping->puerto);
	}

	buffer = malloc(tamano_buffer);

	memcpy(&buffer[posicion],&cantidadMemorias,sizeof(cantidadMemorias));
	posicion += sizeof(cantidadMemorias);

	for(int i=0; i<cantidadMemorias; i++){
		t_memoria* mem_gossiping = list_get(tabla_gossiping,i);

		memcpy(&buffer[posicion],&mem_gossiping->id,sizeof(mem_gossiping->id));
		posicion += sizeof(mem_gossiping->id);

		memcpy(&buffer[posicion],&mem_gossiping->tam_ip,sizeof(mem_gossiping->tam_ip));
		posicion += sizeof(mem_gossiping->tam_ip);

		memcpy(&buffer[posicion],mem_gossiping->ip,mem_gossiping->tam_ip);
		posicion += mem_gossiping->tam_ip;

		memcpy(&buffer[posicion],&mem_gossiping->puerto,sizeof(mem_gossiping->puerto));
		posicion += sizeof(mem_gossiping->puerto);
	}

	send(cliente,buffer,tamano_buffer,0);

	free(buffer);
}

t_list* recibirTablaGossiping(int servidor){

	int bytesRecibidos;
	t_list* tablaRecibida = list_create();

	void* buffer = malloc(1000);

	int cantidadMemorias;

	bytesRecibidos = recv(servidor, buffer, sizeof(cantidadMemorias), 0);
	//printf("bytes recibidos: %d\n",bytesRecibidos);

	if(bytesRecibidos <= 0) {
		perror("Se desconecto el cliente");
		return NULL;
	}

	memcpy(&cantidadMemorias,buffer,sizeof(cantidadMemorias));

	for(int i=0; i<cantidadMemorias; i++){
		t_memoria* mem_gossiping = malloc(sizeof(t_memoria));

		recv(servidor, buffer, sizeof(mem_gossiping->id), 0);
		memcpy(&mem_gossiping->id,buffer,sizeof(mem_gossiping->id));

		recv(servidor, buffer, sizeof(mem_gossiping->tam_ip), 0);
		memcpy(&mem_gossiping->tam_ip,buffer,sizeof(mem_gossiping->tam_ip));

		recv(servidor, buffer, mem_gossiping->tam_ip, 0);
		mem_gossiping->ip = malloc(mem_gossiping->tam_ip);
		memcpy(mem_gossiping->ip,buffer,mem_gossiping->tam_ip);

		recv(servidor, buffer, sizeof(mem_gossiping->puerto), 0);
		memcpy(&mem_gossiping->puerto,buffer,sizeof(mem_gossiping->puerto));

		list_add(tablaRecibida,mem_gossiping);
	}

	free(buffer);

	return tablaRecibida;
}

t_list* obtenerUnion(t_list* lista1, t_list* lista2){
	t_list* listaUnion = list_create();

	for(int i = 0;i<list_size(lista1);i++){
		list_add(listaUnion,list_get(lista1,i));
	}

	for(int i = 0;i<list_size(lista2);i++){
		t_memoria* aux = list_get(lista2,i);

		if(buscarMemoria(listaUnion,aux->id) == NULL){// si encuentra la memoria, no se tiene que agregar a la lista
			list_add(listaUnion,aux);
		}
	}

	return listaUnion;
}

t_memoria* buscarMemoria(t_list* lista,int id) {
	int igualId(t_memoria* p) {
		return p->id == id;
	}

	return list_find(lista, (void*) igualId);
}
