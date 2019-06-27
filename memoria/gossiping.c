/*
 * gossiping.c
 *
 *  Created on: 26 jun. 2019
 *      Author: utnso
 */

#include "gossiping.h"


void procesoGossiping(){
	//int inicio = 1;
	int cliente;
	char** ip_seeds;
	char** puerto_seeds;
	int puerto_seeds_int;
	int activador = 1;

	ip_seeds = obtenerIP_SEEDS();
	puerto_seeds = obtenerPUERTO_SEEDS();
	puerto_seeds_int = atoi(puerto_seeds[0]);



	while(1){
		cliente = conectarseA(ip_seeds[0],puerto_seeds_int);

		if(cliente != 0){
			printf("Si se pudo conectar\n");
			//iniciarGossiping(cliente);

			close(cliente);
		}
		else{
			if(activador){
				t_memoria* nuevo = malloc(sizeof(t_memoria));
				nuevo->ip = strdup("127.0.0.1"); // revisar
				nuevo->puerto = obtenerPuertoConfig();
				nuevo->id = obtenerIdMemoria();
				list_add(tabla_gossiping,nuevo);

				activador = 0;
			}
			//printf("No se pudo conectar\n");
		}

		sleep(3);
	}
}

void iniciarGossiping(int servidor){
	void* buffer = malloc(sizeof(uint8_t));
	memcpy(&buffer[0],GOSSIPING,sizeof(uint8_t));
	send(servidor,buffer,sizeof(uint8_t),0);
	free(buffer);
}

void enviarTablaGossiping(int cliente){
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

t_list* obtenerUnion(t_list* lista1, t_list* lista2){
	t_list* listaUnion = list_create();

	for(int i = 0;i<list_size(lista1);i++){
		list_add(listaUnion,list_get(lista1,i));
	}

	for(int i = 0;i<list_size(lista2);i++){
		t_memoria* aux = list_get(lista2,i);

		if(buscarMemoria(listaUnion,aux->id) != NULL){
			list_add(listaUnion,aux);
		}
	}

	return listaUnion;
}

t_memoria* buscarMemoria(t_list* lista,int id) {
	int igualId(t_memoria* p) {
		return (p->id == id);
	}

	return list_find(lista, (void*) igualId);
}
