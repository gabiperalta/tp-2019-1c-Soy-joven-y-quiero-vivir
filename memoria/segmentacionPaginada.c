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
	t_registro registro;

	memcpy(value,(char*)direccion + sizeof(registro.timestamp) + sizeof(registro.key),MAX_VALUE);

	return value;
}

uint16_t obtenerKey(void* direccion){
	uint16_t key;
	t_registro registro;

	memcpy(&key,(char*)direccion + sizeof(registro.timestamp),MAX_VALUE);

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

/////////////////////////////JOURNAL//////////////////////////////////
t_registroJOURNAL* crearRegistroJOURNAL(char* path, t_pagina* pagina) {

	t_registroJOURNAL* nuevo = malloc(sizeof(t_registroJOURNAL));
	 nuevo->path = strdup(path);
	 nuevo->value = obtenerValue(pagina->direccion);
	 nuevo->timestamp = obtenerTimestamp(pagina->direccion);

	 return nuevo;
}

void filtrarModificados(t_list* listaOriginal, t_list* tabla_segmento){
	bool estaModificada(t_pagina* p){
			int flagEncontrada;
			void* direccion = p->direccion;

			memcpy(&flagEncontrada,(char*)direccion + sizeof(int), sizeof(flagEncontrada));

			if(flagEncontrada == 1){
				return 1;
			}
			else{
				return 0;
			}
		}
	t_list* listaSecundaria = list_filter(tabla_segmento,(void*) estaModificada );
	list_add_all(listaOriginal,listaSecundaria);
}

t_list* quePasarEnElJournal(t_list* tabla_segmentos){

	t_list* listaDeRegistros;
	for(int i = 0; i<tabla_segmentos->elements_count; i++){

		filtrarModificados(list_get(tabla_segmentos,i), listaDeRegistros);

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

////////////COSAS PARA EL LRU//////////////////

t_registro_LRU* crearRegistroLRU(char* path, t_pagina* pagina) {
	 t_registro_LRU* nuevo = malloc(sizeof(t_registro_LRU));
	 nuevo->path = strdup(path);
	 nuevo->numeroPagina = pagina->numeroPagina;
	 nuevo->modificado = pagina->modificado;
	 return nuevo;
}

t_registro_LRU* buscarRegistroLRU(char *path, t_pagina* pagina) {

	int igualRegistro(t_registro_LRU* p) {
		return string_equals_ignore_case(p->path, path) && (p->numeroPagina == pagina->numeroPagina);
	}

	return list_find(lista_LRU, (void*) igualRegistro);
}

void agregarEnListaLRU(char* path, t_pagina* page){

	if (!buscarRegistroLRU(path,page)){ //si no esta
		list_add(lista_LRU, crearRegistroLRU(path, page));
	}
	else {
		int igualRegistro(t_registro_LRU* p) {
			return string_equals_ignore_case(p->path, path) && (p->numeroPagina == page->numeroPagina);
		}
		list_remove_and_destroy_by_condition(lista_LRU,(void*) igualRegistro,(void*) eliminarRegistroLRU);

		list_add(lista_LRU, crearRegistroLRU(path, page));
	}
}

void eliminarRegistroLRU(t_registro_LRU* registro_LRU){
	free(registro_LRU->path);
	free(registro_LRU);
}

/*
void destructorDeSegmentoAUX(t_auxSegmento* auxSeg){
	memset(auxSeg->numeroPagina,0,sizeof(int));
	memset(auxSeg->path,0,sizeof(auxSeg->path));
}

t_auxSegmento* cualTengoQueSacar(t_list* auxLRU){
	bool noModificadoDelSegmento(t_auxSegmento* auxSegmento,t_segmento* segment){
					return (auxSegmento->modificado == 0); ;
				}
	return list_remove_by_condition(auxLRU,(void *) noModificadoDelSegmento);

}

void quitarLuegoDeDrop(t_list* auxLRU,t_segmento *segment){
	list_fold(auxLRU, 0 , (void*) eliminarSegmentoLRU);
}
*/

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

void eliminarSegmento(t_segmento* segment){
	free(segment->path);
	list_destroy_and_destroy_elements(segment->tabla_pagina,(void*) eliminarPagina);
	free(segment);
}

void eliminarPagina(t_pagina* pagina){
	memset(pagina->direccion,NULL,tamano_registro);
	free(pagina);
}

int saberCantidadDePaginasEliminadas(t_segmento* segment){
	return list_size(segment->tabla_pagina);
}

void vaciarMemoria(){

	list_clean_and_destroy_elements(tabla_segmentos,(void*) eliminarSegmento);

	//list_fold(segment->tabla_pagina, 0 , (void*) eliminarSegmento);
	//tambien vacia auxLRU
	// la cambie porque no vaciaba la memo, si no es eso, es el destructor

	// comento la funcion porque me da error
	//list_remove_and_destroy_element(auxLRU,(void*)destructorDeSegmentoAUX);
}

