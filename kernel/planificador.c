/*
 * planificador.c
 *
 *  Created on: 11 jun. 2019
 *      Author: utnso
 */

#include "planificador.h"

void ejecutar(t_queue* queueRequests){

}

void crearEstructura(t_nueva_request* request){
	t_queue * request_string = queue_create();

	switch(request->id){
		case SELECT:
		case INSERT:
		case CREATE:
		case DESCRIBE:
		case DROP:

			queue_push(request_string,crearRequestString(request->nuevaRequestString));
			queue_push(queue_listo,request_string);

			break;
		case RUN:

			queue_push(queue_listo,leerArchivo(gestionarSolicitud(request->nuevaRequestString).path_archivo));

			break;
		default:
			break;
	}
}

t_queue* leerArchivo(char * pathArchivo){

	FILE * archivo = fopen(pathArchivo,"r");
	char * auxiliar = malloc(80);
	t_queue * request_string = queue_create();

	//películas.lql
	// RUN /home/utnso/Escritorio/pruebas/tp/animales.lql

	if(archivo == NULL) {
		printf("No se abrio el archivo\n");
	}

	//while(!feof(archivo)){
	while(fgets(auxiliar, 60, archivo) != NULL){

		//token = strtok(auxiliar,"\n");
		//printf("%s\n",token);

		//fgets(auxiliar, 60, archivo);
		auxiliar[strcspn(auxiliar,"\n")] = 0;
		printf("%s\n",auxiliar);

		queue_push(request_string,crearRequestString(auxiliar));

	}

	free(auxiliar);
	fclose(archivo);

	return request_string;
}


char* crearRequestString(char* requestLeido){
	char* requestString = malloc(strlen(requestLeido));
	strcpy(requestString,requestLeido);
	return requestString;
}
