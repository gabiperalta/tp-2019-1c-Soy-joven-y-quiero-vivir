/*
 * funciones.c
 *
 *  Created on: 12 jun. 2019
 *      Author: utnso
 */

#include "funcionesKernel.h"

void procesarRequest(uint8_t id,char* requestString){
	switch(id){
		case SELECT:
		case INSERT:
		case CREATE:
		case DESCRIBE:
		case DROP:
		case RUN:
			t_nueva_request* nuevaRequest;
			nuevaRequest->id = id;
			nuevaRequest->nuevaRequestString = malloc(strlen(requestString));
			strcpy(nuevaRequest->nuevaRequestString,requestString);

			queue_push(queue_listo,nuevaRequest);
			break;
		case JOURNAL:
			break;
		case ADD:
			break;
		case METRICS:
			break;
		default:
			break;
	}
}
