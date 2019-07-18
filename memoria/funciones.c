/*
 * funciones.c
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#include "funciones.h"

void consola(){

	t_request request_ingresada;

	system("clear");
	printf("------------ MEMORIA ----------------\n");

	char * linea;
	while(1) {
		linea = readline(">");
		if(linea)
			add_history(linea);
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}

		sem_wait(&mutexEscrituraMemoria);
		request_ingresada = gestionarSolicitud(linea);
		procesarRequest(request_ingresada);
		liberarMemoriaRequest(request_ingresada);
		sem_post(&mutexEscrituraMemoria);

		free(linea);
	}

	liberarRecursos();
}

void prueba(void* memoria,t_list* tabla_segmentos){

	t_segmento* segmento_retornado;
	t_registro registro;

	registro.key = 16;
	registro.timestamp = 237;
	registro.value = "Espero que funcione";

	list_add(tabla_segmentos,crearSegmento("TABLA1"));
	segmento_retornado = (t_segmento*)list_get(tabla_segmentos,0);

	list_add(segmento_retornado->tabla_pagina,crearPagina(list_size(segmento_retornado->tabla_pagina),0,registro));

}

// nuevo procesar request, ahora retorna un t_response.
// si algo falla, volver a la funcion original (buscarla en un commit viejo).
// por ahora solo funciona con INSERT y SELECT(cuando el valor ya estaba en memoria)
// al toke
t_response procesarRequest(t_request request){
	t_segmento* segmento_encontrado;
	t_pagina* pagina_encontrada;
	char* valueObtenido = malloc(MAX_VALUE);
	t_registro registroNuevo;

	//t_response respuestaFS;
	t_response response;

	switch(request.header){
		case 1://SELECT TABLA1 16

			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);

			if(segmento_encontrado != NULL){
				pagina_encontrada = buscarPagina(segmento_encontrado->tabla_pagina,request.key);

				if(pagina_encontrada != 0){
					valueObtenido = obtenerValue(pagina_encontrada->direccion);
					printf("%s\n",valueObtenido);
					log_info(logMemoria, "Se ha seleccionado un value que estaba en memoria");

					// respuesta que se envia al kernel
					response.header = SELECT_R;
					response.tam_value = strlen(valueObtenido) + 1;
					response.value = malloc(response.tam_value);
					response.timestamp = 0; // al kernel no le importa el timestamp
					strcpy(response.value,valueObtenido);
				}
				else if(pagina_encontrada == NULL){
					//enviarFS(request);
					//respuestaFS = recibirResponse(conectarseA(IP_LOCAL, 40904));


/*~~~~~~~~~~ PREGUNTA TONTA: EL IP DE DONDE OBTENGO LA RESPUESTA ES LA MISMA A LA CUAL ENVIO ~~~~~~~~~~~~~*/
	//TODO CAMBIAR LO DE ABAJO PARA QUE USE LA PESPUESTA
					/*int posicionSegmentoNuevo;
					t_segmento* segmento_nuevo;
					posicionSegmentoNuevo = list_add(tabla_segmentos,crearSegmento(request.nombre_tabla));
					segmento_nuevo = (t_segmento*)list_get(tabla_segmentos,posicionSegmentoNuevo);
					list_add(segmento_nuevo->tabla_pagina,crearPagina(0,1,memoria,registroNuevo));*/
					//cantPaginasLibres--;
					//log_info(logMemoria, "Se ha seleccionado un value que NO estaba en memoria");
// respuesta que se envia al kernel

				}
			}else if(segmento_encontrado== NULL){
				//enviarFS(request);
				//respuestaFS = recibirResponse(conectarseA(IP_LOCAL, 40904));
	//TODO CAMBIAR LO DE ABAJO PARA QUE USE LA PESPUESTA
				/*int posicionSegmentoNuevo;
				t_segmento* segmento_nuevo;
				posicionSegmentoNuevo = list_add(tabla_segmentos,crearSegmento(request.nombre_tabla));
				segmento_nuevo = (t_segmento*)list_get(tabla_segmentos,posicionSegmentoNuevo);
				list_add(segmento_nuevo->tabla_pagina,crearPagina(0,1,memoria,registroNuevo));*/
				//cantPaginasLibres--;
				//log_info(logMemoria, "Se ha seleccionado un value que NO estaba en memoria");
	// respuesta que se envia al kernel
			}

			break;
		case 2://INSERT
			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);
			registroNuevo.key = request.key;
			registroNuevo.value = request.value;
			registroNuevo.timestamp = getCurrentTime();

			if (segmento_encontrado!= NULL){
				pagina_encontrada = buscarPagina(segmento_encontrado->tabla_pagina,registroNuevo.key);

				if(pagina_encontrada != NULL){

					uint32_t timestampObtenido = obtenerTimestamp(pagina_encontrada->direccion);

					if(timestampObtenido < registroNuevo.timestamp){//se actualiza el value
						actualizarRegistro(pagina_encontrada, registroNuevo);
						log_info(logMemoria, "Se ha insertado un value.");
					}
					else if (timestampObtenido >= registroNuevo.timestamp){
						log_info(logMemoria, "No se actualizo el value porque tiene un timestamp menor.");
					}
				}
				else if (pagina_encontrada == NULL){// si no la encuentra

					/*if(cantPaginasLibres > 0){
					*/
					list_add(segmento_encontrado->tabla_pagina,crearPagina(list_size(segmento_encontrado->tabla_pagina),1,registroNuevo));
					//cantPaginasLibres--;
					log_info(logMemoria, "Se ha insertado un value.");
					/*
					 * }
					 */
					/*else{
						//hacerJournaling(tabla_segmentos);
						 * COPIAR LO QUE DICE EN EL CASE DE JOURNAL
					}*/
				}
			}
			else if (segmento_encontrado == NULL){ // no se encontro el segmento
				int posicionSegmentoNuevo;
				t_segmento* segmento_nuevo;

				posicionSegmentoNuevo = list_add(tabla_segmentos,crearSegmento(request.nombre_tabla));
				segmento_nuevo = (t_segmento*)list_get(tabla_segmentos,posicionSegmentoNuevo);
				list_add(segmento_nuevo->tabla_pagina,crearPagina(0,1,registroNuevo));
				//cantPaginasLibres--;
				log_info(logMemoria, "Se ha insertado un value.");
			}

			response.header = INSERT_R;

			break;
		case 3://CREATE
		
			//enviarFS(request);
			//respuestaFS = recibirResponse(conectarseA(IP_LOCAL, 40904));
			//log_info(logMemoria, "Se ha creado una table en el FS.");

			printf("tabla %s\n",request.nombre_tabla);
			printf("tipo consistencia %d\n",request.tipo_consistencia);
			printf("particiones %d\n",request.numero_particiones);
			printf("compaction time %d\n\n",request.compaction_time);

			response.header = CREATE_R;

			break;
		case 4://DESCRIBE

			//enviarFS(request);
			//respuestaFS = recibirResponse(conectarseA(IP_LOCAL, 40904));
			//log_info(logMemoria, "Se ha obtenido la metadata del FS.");
			//response = respuestaFS;

			break;
		case 5://DROP
			//enviarFS(request);
			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);
			if(segmento_encontrado!= NULL){
				//cantPaginasLibres-= saberCantidadDePaginasEliminadas(segmento_encontrado);
				//eliminarSegmento(tabla_segmentos,segmento_encontrado);
				//log_info(logMemoria, "Se ha eliminado un segmento.");
			}
			response.header = DROP_R;
			break;
		case 6://JOURNAL
			//enviarCantidadDeJournal(40904,(cantTotalPaginas - cantPaginasLibres));
			//enviarMEMOaFS(cachoMemo);
			//hacerJournaling(tabla_segmentos);
			//cantPaginasLibres= cantTotalPaginas;
			//log_info(logMemoria, "Se ha hecho un journal.");
			response.header = JOURNAL_R;

			break;
	}

	free(valueObtenido);

	return response;
}

void servidor(){
	void * conectado;
	while((conectado=aceptarConexion(puerto))!= 1){

		//printf("Se acepto conexion\n");
		pthread_t hiloRequest;
		pthread_create(&hiloRequest,NULL,(void*)atenderRequest,conectado);
		pthread_detach(hiloRequest);
	}
}



void atenderRequest(void* cliente){
	pthread_mutex_lock(&mutex);
	t_request request_ingresada = recibirRequest(cliente);
	t_response response_generado;

	while(request_ingresada.error != 1){

		if(request_ingresada.header == GOSSIPING){
			enviarTablaGossiping(cliente);
			printf("Se envio la tabla\n");
			break;
		}
		else{
			sem_wait(&mutexEscrituraMemoria);
			//procesarRequest(request_ingresada);
			response_generado = procesarRequest(request_ingresada);

			/*
			printf("%d ",request_ingresada.key);
			printf("%s ",request_ingresada.nombre_tabla);
			if(request_ingresada.header == 2){
				printf("%s",request_ingresada.value);
			}
			printf("\n");
			*/

			sem_post(&mutexEscrituraMemoria);
			liberarMemoriaRequest(request_ingresada);

			// se envia el response generado
			enviarResponse(cliente,response_generado);

			request_ingresada = recibirRequest(cliente);
		}

	}

	printf("\n");

	close(cliente);
	pthread_mutex_unlock(&mutex);
}
void enviarFS(t_request request){
	int servidor = conectarseA(IP_LOCAL, 40904);
	enviarRequest(servidor,request);
	close(servidor);
}

int obtenerPuertoConfig(){
	return config_get_int_value(archivo_config,"PUERTO");
}

int obtenerTamanioMemo(){
	return config_get_int_value(archivo_config,"TAM_MEM");
}

//tipoRetardo = RETARDO_GOSSIPING RETARDO_JOURNAL RETARDO_MEM RETARDO_FS
void modificarRetardos(char* tipoRetardo,int valorNuevo){
	  //SE SUPONE QUE ESTA ESTA, QUISE COPIAR LA DE LLAS COMMONS Y LA VERDAD QUE MEZCLA DE TOdo
	    char* nuevoRetardo;
	    snprintf(nuevoRetardo, 10, "%d", valorNuevo);
	    config_set_value(archivo_config, tipoRetardo, nuevoRetardo);
	    config_save(archivo_config);
}

int obtenerRetardo(char* tipoRetardo){
	return config_get_int_value(archivo_config,tipoRetardo);
}

char* obtenerIP_FS(){
	return config_get_string_value(archivo_config,"IP_FS");
}

int obtenerPuertoFS(){
	return config_get_int_value(archivo_config,"PUERTO_FS");
}

char** obtenerIP_SEEDS(){
	return config_get_array_value(archivo_config,"IP_SEEDS");
}

char** obtenerPUERTO_SEEDS(){
	return config_get_array_value(archivo_config,"PUERTO_SEEDS");
}

int obtenerIdMemoria(){
	return config_get_int_value(archivo_config,"MEMORY_NUMBER");
}

void liberarRecursos(){
	free(memoria);
	close(puerto);
	config_destroy(archivo_config);
	log_destroy(logMemoria);
}

int cantidadDePaginas(int tamanioMemo){
	int cantPaginas = tamanioMemo / sizeof(t_pagina);
	return cantPaginas;
}
/*char* obtenerPath() {
    char buf[PATH_MAX + 1];
    char *res = realpath("memoria.config", buf);
    return res;
}// no se si hace lo que quiero
*/

///////////////////////LOG/////////////////////////////////

void inicializarLogMemo(){
	logMemoria = log_create(PATH_LOG,"memoria",false,LOG_LEVEL_INFO);
}


