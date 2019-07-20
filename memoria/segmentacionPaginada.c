/*
 * segmentacionPaginada.c
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#include "segmentacionPaginada.h"

t_segmento* crearSegmento(char *path) {
	 t_segmento* nuevo = malloc(sizeof(t_segmento));
	 nuevo->path = strdup(path);
	 nuevo->tabla_pagina = list_create();
	 return nuevo;
}

t_pagina* crearPagina(int numeroPagina,int modificado,t_registro registro) {
    t_pagina* new = malloc(sizeof(t_pagina));
    new->numeroPagina = numeroPagina;
    new->modificado = modificado;
    new->direccion = guardarRegistro(registro);
    return new;
}
t_registro* crearRegistro(t_response respuesta, uint16_t keyNuevo){
	t_registro* nuevo = malloc(sizeof(t_registro));
	nuevo->key = keyNuevo;
	nuevo->timestamp = respuesta.timestamp;
	nuevo->value = respuesta.value ;
	return nuevo;
}

t_segmento* buscarSegmento(t_list* lista,char *path) {
	int igualPath(t_segmento *p) {
		return string_equals_ignore_case(p->path, path);
	}

	return list_find(lista, (void*) igualPath);
}

t_pagina* buscarPagina(t_list* lista,uint16_t key) {
	int igualKey(t_pagina* p){

		uint16_t keyEncontrada;
		void* direccion = p->direccion;

		memcpy(&keyEncontrada,(char*)direccion + sizeof(int), sizeof(keyEncontrada));

		if(keyEncontrada == key){
			return 1;
		}
		else{
			return 0;
		}
	}

	return list_find(lista, (void*) igualKey);
}

void* guardarRegistro(t_registro registro){
	int posicionAnalizada = 0;
	void * direccion;
	int posicion = 0;
	int lugarVacio = 0;

	while(lugarVacio != 1){
		direccion = (char*) memoria + posicionAnalizada;

		if(obtenerTimestamp(direccion) == 0){
			memcpy(direccion,&registro.timestamp,sizeof(registro.timestamp));
			posicion += sizeof(registro.timestamp);
			memcpy(&direccion[posicion],&registro.key,sizeof(registro.key));
			posicion += sizeof(registro.key);
			memcpy(&direccion[posicion],registro.value,strlen(registro.value) + 1);
			//posicion += VALUE;

			lugarVacio = 1;

			return direccion;
		}

		posicionAnalizada += MAX_VALUE + sizeof(registro.timestamp) + sizeof(registro.key);
	}
}

void actualizarRegistro(t_pagina* pagina,t_registro registro){
	int posicion = sizeof(registro.timestamp) + sizeof(registro.key);
	void* direccion = pagina->direccion;

	pagina->modificado = 1;

	memset(direccion,0,sizeof(registro.timestamp));
	memcpy(direccion,&registro.timestamp,sizeof(registro.timestamp));

	memset(&direccion[posicion],0,MAX_VALUE);
	memcpy(&direccion[posicion],registro.value,strlen(registro.value)+1);
}


char* obtenerValue(void* direccion){
	char* value = malloc(30);

	memcpy(value,(char*)direccion + 6,MAX_VALUE);

	return value;
}
char* obtenerKey(void* direccion){
	char* key = malloc(30);

		memcpy(key,(char*)direccion + 6,MAX_VALUE);

		return key;
}

uint32_t obtenerTimestamp(void* direccion){
	//int* timestamp = malloc(30);
	uint32_t timestamp = 0;

	memcpy(&timestamp,(char*)direccion,sizeof(uint32_t));

	return timestamp;
}

uint32_t getCurrentTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}
t_registroJOURNAL* crearRegistroJOURNAL(char* path, t_pagina* pagina) {

	t_registroJOURNAL* nuevo = malloc(sizeof(t_registroJOURNAL));
	 nuevo->path = strdup(path);
	 nuevo->value = obtenerValue(pagina->direccion);
	 nuevo->key =
	 nuevo->timestamp = obtenerTimestamp(pagina->direccion);

	 return nuevo;
}
t_list* quePasarEnElJournal(t_list* tabla_segmentos){

	t_list* listaDeRegistros;
	for(int i = 0; i<tabla_segmentos->elements_count; i++){



			//	list_add(listaDeRegistros, registro);
			}
	return listaDeRegistros;
}

t_pagina* buscarPaginaModificadaONO(t_list* lista, int flag) {
	int estaModificada(t_pagina* p, int flag){
		int flagEncontrada;
		void* direccion = p->direccion;

		memcpy(&flagEncontrada,(char*)direccion + sizeof(int), sizeof(flagEncontrada));

		if(flagEncontrada == flag){
			return 1;
		}
		else{
			return 0;
		}
	}
	return list_find(lista, (void*) estaModificada);
}
t_list* todasPaginasModificadas(t_list* tabla_segmentos){


return 0;
}
//COSAS PARA EL LRU

t_auxSegmento* crearAuxSegmento(char* path, t_pagina* pagina) {
	 t_auxSegmento* nuevo = malloc(sizeof(t_auxSegmento));
	 nuevo->path = strdup(path);
	 nuevo->numeroPagina = pagina->numeroPagina;
	 nuevo->modificado = pagina->modificado;
	 return nuevo;
}

void agregarEnListaLRU(t_list* auxLRU,t_segmento *segment, t_pagina *page){

	if (!buscarSegmento(auxLRU,segment->path)){ //si no esta
	t_auxSegmento* nuevo = crearAuxSegmento(segment->path, page);
	list_add(auxLRU, nuevo);
	} else {
		bool buscador(t_segmento* segmento){
			return buscarSegmento(auxLRU,segmento->path);
		}

		t_auxSegmento* yaUsado = list_remove_by_condition(auxLRU, (void*) buscador);
		list_add(auxLRU, yaUsado);
	}
}
void destructorDeSegmentoAUX(t_auxSegmento* auxSeg){
	memset(auxSeg->numeroPagina,0,sizeof(int));
	memset(auxSeg->path,0,sizeof(auxSeg->path));
}

void eliminarSegmentoLRU(t_list* auxLRU, t_segmento* segment){
	bool buscador(t_segmento* segmento){
				return buscarSegmento(auxLRU,segment->path);
			}
	list_remove_and_destroy_by_condition(auxLRU,(void*) buscador, (void*) destructorDeSegmentoAUX);
}

t_auxSegmento* cualTengoQueSacar(t_list* auxLRU, t_segmento* segment){
	bool noModificadoDelSegmento(t_auxSegmento* auxSegmento,t_segmento* segment){
					return (auxSegmento->modificado == 0) && buscarSegmento(auxLRU, segment->path); ;
				}
	return list_remove_by_condition(auxLRU,(void *) noModificadoDelSegmento);

}

void quitarLuegoDeDrop(t_list* auxLRU,t_segmento *segment){
	list_fold(auxLRU, 0 , (void*) eliminarSegmentoLRU);
}
//esto es para eliminar de memoria de veritas
void destructorDePagina(t_pagina* pagina){
	memset(pagina->direccion,0,sizeof(pagina));
}
void destructorDeSegmento(t_segmento segment){ /// ES PUNTERO O NO
	free(segment.path);
	list_destroy_and_destroy_elements(segment.tabla_pagina,(void*) destructorDePagina);
}
t_pagina* buscarPaginaPorNumero(t_list* lista, int numeroPagina) {
	int estaELNumero(t_pagina* p, int numero){
			int numeroEncontrada;
			void* direccion = p->direccion;

			memcpy(&numeroEncontrada,(char*)direccion + sizeof(int), sizeof(numeroEncontrada));

			if(numeroEncontrada == numero){
				return 1;
			}
			else{
				return 0;
			}
		}
		return list_find(lista, (void*) estaELNumero);

}

void eliminarSegmento(t_list* lista, t_segmento* segment){
	bool buscador(t_segmento* segmento){

				return buscarSegmento(lista,segment->path);
			}
	list_remove_and_destroy_by_condition(lista,(void*) buscador, (void*) destructorDePagina);
}
void eliminarPagina(t_segmento* segment, t_pagina* pagina){
	bool buscador(t_segmento* segment,t_pagina* pagina){

					return buscarPagina(segment->tabla_pagina, obtenerKey(pagina->direccion));
				}
		list_remove_and_destroy_by_condition(segment->tabla_pagina,(void*) buscador, (void*) destructorDePagina);
}

int saberCantidadDePaginasEliminadas(t_segmento* segment){
	return list_size(segment->tabla_pagina);
}

void vaciarMemoria(t_segmento* segment, t_list* auxLRU){
	list_fold(segment->tabla_pagina, 0 , (void*) eliminarSegmento);
	//tambien vacia auxLRU
	// la cambie porque no vaciaba la memo, si no es eso, es el destructor

	// comento la funcion porque me da error
	//list_remove_and_destroy_element(auxLRU,(void*)destructorDeSegmentoAUX);
}
