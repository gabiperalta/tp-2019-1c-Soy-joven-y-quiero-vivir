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
#include <stdint.h>

#define SELECT 		1
#define INSERT 		2
#define CREATE 		3
#define DESCRIBE 	4
#define DROP 		5
#define JOURNAL 	6
#define ADD 		7
#define RUN 		8
#define METRICS		9

#define SC			1
#define SHC			2
#define EC			3


typedef struct{

	uint8_t header;
	uint8_t tam_nombre_tabla;
	char* nombre_tabla;
	uint16_t key;
	uint8_t tam_value;
	char* value;
	uint32_t timestamp;
	uint8_t tipo_consistencia;
	uint16_t numero_particiones;
	uint64_t compaction_time;
	char* path_archivo;
	int error;

}t_request;

t_request gestionarSolicitud(char* solicitud);
void liberarMemoriaRequest(t_request request);
int obtenerTipoConsistencia(char * consistencia);

#endif /* BIBLIOTECA_H_ */
