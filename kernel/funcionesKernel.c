/*
 * funciones.c
 *
 *  Created on: 12 jun. 2019
 *      Author: utnso
 */

#include "funcionesKernel.h"

void procesarRequest(uint8_t id,char* requestString){
	t_nueva_request* nuevaRequest = malloc(sizeof(t_nueva_request));
	t_request request;

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
			request = gestionarSolicitud(requestString);
			agregarMemoria(request.id_memoria,request.tipo_consistencia);
			//liberarMemoriaRequest(request);
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
		pthread_detach(hiloEjecutar);

		//sem_wait(&semaforoExecOcupado);
	}
}

void ejecutar(t_queue* script){
	int quantumActual = quantum;
	t_request requestEjecutar;
	t_memoria memoriaObtenida;

	int servidor = conectarseA(ip_memoria, puerto_memoria);// conexion casera

	while(quantumActual > 0){

		if(queue_is_empty(script)){
			break;
		}

		requestEjecutar = gestionarSolicitud(queue_pop(script));

		//memoriaObtenida = obtenerMemoria();

		printf("%d ",requestEjecutar.key);
		printf("%s ",requestEjecutar.nombre_tabla);
		if(requestEjecutar.header == 2){
			printf("%s",requestEjecutar.value);
		}
		printf("\n");

		//int servidor = conectarseA(IP_LOCAL, PUERTO_ESCUCHA_MEM);// conexion casera
		enviarRequest(servidor,requestEjecutar);
		//close(servidor);

		liberarMemoriaRequest(requestEjecutar);
		quantumActual--;
	}

	//close(servidor);

	printf("\n");
	sleep(3); // cambiarlo por el valor del config

	close(servidor);

	if(queue_is_empty(script) == 0){
		queue_push(queue_listo,script);
		sem_post(&semaforoListo);
	}
	//queue_push(queue_listo,script);

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

			//printf("%s\n",request->nuevaRequestString);

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

	printf("%s\n",pathArchivo);

	FILE * archivo = fopen(pathArchivo,"r");
	char * auxiliar = malloc(100);
	t_queue * request_string = queue_create();

	// RUN /home/utnso/Escritorio/pruebas/tp/comidas.lql
	// RUN /home/utnso/Escritorio/pruebas/tp/animales.lql
	// RUN /home/utnso/Escritorio/pruebas/tp/misc_1.lql
	// RUN /home/utnso/Escritorio/pruebas/tp/películas.lql

	if(archivo == NULL) {
		printf("No se abrio el archivo\n");
	}

	//while(!feof(archivo)){
	while(fgets(auxiliar, 100, archivo) != NULL){

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
	char* requestString = malloc(strlen(requestLeido)+1);
	strcpy(requestString,requestLeido);
	return requestString;
}

void leerArchivoConfig(){
	multiprocesamiento = config_get_int_value(archivo_config,"MULTIPROCESAMIENTO");
	quantum = config_get_int_value(archivo_config,"QUANTUM");
	ip_memoria = config_get_string_value(archivo_config,"IP_MEMORIA");
	puerto_memoria = config_get_int_value(archivo_config,"PUERTO_MEMORIA");
}

void agregarMemoria(int idMemoria, uint8_t tipoConsistencia){

	switch(tipoConsistencia){
		case SC:
			if(list_size(criterio_SC) < 1){ // solo debe haber una memoria para SC
				sem_wait(&mutexCriterio);
				list_add(criterio_SC,idMemoria);
				sem_post(&mutexCriterio);
			}

			break;
		case SHC:
			break;
		case EC:
			sem_wait(&mutexCriterio);
			list_add(criterio_EC,idMemoria);
			sem_post(&mutexCriterio);

			break;
	}
}

t_memoria* obtenerMemoria(char* nombreTabla){
	//rand() %


	return 0;
}
