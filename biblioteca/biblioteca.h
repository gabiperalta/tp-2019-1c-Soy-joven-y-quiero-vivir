/*
 * biblioteca.h
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#ifndef BIBLIOTECA_H_
#define BIBLIOTECA_H_

#include <string.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

	uint8_t header;
	uint8_t tam_nombre_tabla;
	char* nombre_tabla;
	uint16_t key;
	char* value;
	uint8_t tam_value;
	uint8_t tipo_consistencia;
	uint16_t numero_particiones;
	uint64_t compaction_time;

}t_request;

t_request gestionarSolicitud(char* solicitud);

#endif /* BIBLIOTECA_H_ */

/*
 * #define SELECT 1
#define CREATE 2
#define DESCRIBE 3
#define DROP 4
#define INSERT 5
#define JORUNAL 6
#define ADD 7
 */
