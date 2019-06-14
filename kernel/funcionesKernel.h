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

void procesarRequest(uint8_t id,char* requestString);
void atenderNuevos();
void atenderListos();
void ejecutar(t_queue* script);
void crearEstructura(t_nueva_request* request);
//t_queue* crearEstructura(char * pathArchivo);
t_queue* leerArchivo(char * pathArchivo);
char* crearRequestString(char* requestLeido);

#endif /* FUNCIONESKERNEL_H_ */
