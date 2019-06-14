/*
 * funciones.c
 *
 *  Created on: 12 jun. 2019
 *      Author: utnso
 */

#include "funcionesKernel.h"

void procesarRequest(uint8_t id,char* requestString){
	t_nueva_request* nuevaRequest = malloc(sizeof(t_nueva_request));

	switch(id){
		case SELECT:
		case INSERT:
		case CREATE:
		case DESCRIBE:
		case DROP:
		case RUN:
			nuevaRequest->id = id;
			nuevaRequest->nuevaRequestString = malloc(strlen(requestString));
			strcpy(nuevaRequest->nuevaRequestString,requestString);

			sem_wait(&mutexNuevo);
			queue_push(queue_nuevo,nuevaRequest);
			sem_post(&mutexNuevo);
			sem_post(&semaforoNuevo);
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

void atenderNuevos(){
	while(1){

		sem_wait(&semaforoNuevo);
		sem_wait(&mutexNuevo);

		crearEstructura((t_nueva_request*)queue_pop(queue_nuevo));

		sem_post(&mutexNuevo);
	}
}

void atenderListos(){
	while(1){

		sem_wait(&semaforoListo);
		sem_wait(&semaforoExecLibre);

		pthread_t hiloEjecutar;
		pthread_create(&hiloEjecutar,NULL,(void*)ejecutar,queue_pop(queue_listo));
		pthread_detach(&hiloEjecutar);

		//sem_wait(&semaforoExecOcupado);
	}
}

void ejecutar(t_queue* script){
	int quantum = 1;
	t_request requestEjecutar;

	while(quantum > 0){

		requestEjecutar = gestionarSolicitud(queue_pop(script));

		//printf("%d\n",requestEjecutar.key);

		int servidor = conectarseA(IP_LOCAL, PUERTO_ESCUCHA_MEM);// conexion casera
		enviarRequest(servidor,requestEjecutar);
		close(servidor);

		quantum--;
	}



	sem_post(&semaforoExecLibre);
}

void crearEstructura(t_nueva_request* request){
	t_queue * request_string = queue_create();

	switch(request->id){
		case SELECT:
		case INSERT:
		case CREATE:
		case DESCRIBE:
		case DROP:
			sem_wait(&mutexListo);

			queue_push(request_string,crearRequestString(request->nuevaRequestString));
			queue_push(queue_listo,request_string);

			sem_post(&mutexListo);
			sem_post(&semaforoListo);

			break;
		case RUN:

			sem_wait(&mutexListo);
			queue_push(queue_listo,leerArchivo(gestionarSolicitud(request->nuevaRequestString).path_archivo));
			sem_post(&mutexListo);
			sem_post(&semaforoListo);

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