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

#include "../biblioteca/biblioteca_sockets.h"
#include "segmentacionPaginada.h"

typedef struct{

	uint8_t clave;
	uint8_t tam_nombre_tabla;
	char* nombre_tabla;

}request;

int gestionarFuncionKernel(char* solicitud);
void consola();
void recibirPaquete(int socketCliente);

#endif /* FUNCIONES_H_ */
