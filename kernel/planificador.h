/*
 * planificador.h
 *
 *  Created on: 11 jun. 2019
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "kernel.h"

typedef struct{

	uint8_t id;
	char* nuevaRequestString;

}t_nueva_request;


t_queue* crearEstructura(char * pathArchivo);
t_queue* leerArchivo(char * pathArchivo);
char* crearRequestString(char* requestLeido);

#endif /* PLANIFICADOR_H_ */
