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
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/log.h>

#define SELECT 				1
#define INSERT 				2
#define CREATE 				3
#define DESCRIBE 			4
#define DROP 				5
#define JOURNAL 			6
#define ADD 				7
#define RUN 				8
#define METRICS				9
#define GOSSIPING			10
#define GOSSIPING_KERNEL	11

#define SC			1
#define SHC			2
#define EC			3

#define SELECT_R 		1
#define INSERT_R 		2
#define CREATE_R		3
#define DESCRIBE_R		4
#define DROP_R			5
#define JOURNAL_R		6
#define ERROR_R			7
#define CANT_DESCRIBE_R	8
#define CANT_JOURNAL_R	9
#define FULL_R			10


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
	int id_memoria;
	int error;

}t_request;

typedef struct{

	uint8_t header;
	uint8_t tam_value;
	char* value;
	uint32_t timestamp;
	uint8_t tam_nombre_tabla;
	char* nombre_tabla;
	uint8_t tipo_consistencia;
	uint16_t numero_particiones;
	uint64_t compaction_time;
	uint8_t cantidad_describe;
	t_list* lista;
	int error;

}t_response;

t_request gestionarSolicitud(char* solicitud);
void liberarMemoriaRequest(t_request request);
void liberarMemoriaResponse(t_response response);
int obtenerTipoConsistencia(char * consistencia);
t_log* iniciar_logger(char* nombreArchivo, char* nombreProceso);
uint8_t cantidadElementosCharAsteriscoAsterisco(char** array);

#endif /* BIBLIOTECA_H_ */
