/*
 * funcionesKernel.h
 *
 *  Created on: 12 jun. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESKERNEL_H_
#define FUNCIONESKERNEL_H_


#include "kernel.h"

typedef struct{

	uint8_t id;
	char* nuevaRequestString;

}t_nueva_request;

// ver despues si conviene unificarlo _todo en gossiping
typedef struct{

	int id;
	uint8_t tam_ip;
	char* ip;
	int puerto;

}t_memoria;

typedef struct{

	uint8_t tam_nombre_tabla;
	char* nombre_tabla;
	uint8_t tipo_consistencia;
	uint16_t numero_particiones;
	uint64_t compaction_time;

}t_tabla;

void procesarRequest(uint8_t id,char* requestString);
void atenderNuevos();
void atenderListos();
void ejecutar(t_queue* script);
void crearEstructura(t_nueva_request* request);
//t_queue* crearEstructura(char * pathArchivo);
t_queue* leerArchivo(char * pathArchivo);
char* crearRequestString(char* requestLeido);
void leerArchivoConfig();
void agregarMemoria(int idMemoria, uint8_t tipoConsistencia);
t_memoria* obtenerMemoria(char* nombreTabla);
t_tabla* buscarTabla(char* nombreTabla);
void agregarTabla(t_response tablaRecibida);
t_memoria* buscarMemoria(t_list* lista,int id);

#endif /* FUNCIONESKERNEL_H_ */
