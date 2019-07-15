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

void procesarRequest(t_request request){
	t_segmento* segmento_encontrado;
	t_pagina* pagina_encontrada;
	char* valueObtenido = malloc(MAX_VALUE);
	t_registro registroNuevo;
	/*t_pagina* pagina_nueva;
	int servidor;
	ESTAN SIN USAR, NO SE SI LAS PUSE YO O QUIEN,
	PERO SI VOS NO LAS PUSISTE BORRALAS*/


	switch(request.header){
		case 1://SELECT TABLA1 16

			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);

			if(segmento_encontrado != NULL){
				pagina_encontrada = buscarPagina(segmento_encontrado->tabla_pagina,request.key);

				if(pagina_encontrada != 0){
					valueObtenido = obtenerValue(pagina_encontrada->direccion);
					printf("%s\n",valueObtenido);
					log_info(logMemoria, "Se ha seleccionado un value que estaba en memoria");
				}
			}else if(segmento_encontrado== NULL){
				//enviarFS(request);
				//RECIBIR VALUE SOLICITADO
				//LO DE ABAJO SERIA GUARDAR LO QUE NOS ENVIARON
				/*int posicionSegmentoNuevo;
				t_segmento* segmento_nuevo;
				posicionSegmentoNuevo = list_add(tabla_segmentos,crearSegmento(request.nombre_tabla));
				segmento_nuevo = (t_segmento*)list_get(tabla_segmentos,posicionSegmentoNuevo);
				list_add(segmento_nuevo->tabla_pagina,crearPagina(0,1,memoria,registroNuevo));*/
				//cantPaginasLibres--;
				//log_info(logMemoria, "Se ha seleccionado un value que NO estaba en memoria");
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
					//valueObtenido = obtenerValue(pagina_encontrada->direccion);
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

					//if(cantPaginasLibres > 0){}

					list_add(segmento_encontrado->tabla_pagina,crearPagina(list_size(segmento_encontrado->tabla_pagina),1,registroNuevo));
					//cantPaginasLibres--;
					log_info(logMemoria, "Se ha insertado un value.");

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

			break;
		case 3://CREATE
		
			//enviarFS(request);
			//RECIBIR DE FS EL OK O EL ERROR
			//printf(LO_RECIBIDO);
			//log_info(logMemoria, "Se ha creado una table en el FS.");
			break;
		case 4://DESCRIBE

			//enviarFS(request);
			//RECIBIR DE FS EL OK O EL ERROR
			//printf(LO_RECIBIDO);
			//log_info(logMemoria, "Se ha obtenido la metadata del FS.");
			break;
		case 5://DROP
			//enviarFS(request);
			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);
			if(segmento_encontrado!= NULL){
				//eliminarSegmento(segmento_encontrado);
				//saberCantidadDePaginasEliminadas()
				//cantPaginasLibres-= la cant anterior;
				//log_info(logMemoria, "Se ha eliminado un segmento.");
			}

			break;
		case 6://JOURNAL
			//hacerJournaling();
			//segun entendi se borra todo
			//cantPaginasLibres vuelve al valor original.
			//log_info(logMemoria, "Se ha hecho un journal.");
			break;
	}

	free(valueObtenido);
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

	while(request_ingresada.error != 1){

		if(request_ingresada.header == GOSSIPING){
			enviarTablaGossiping(cliente);
			printf("Se envio la tabla\n");
			break;
		}
		else{
			sem_wait(&mutexEscrituraMemoria);
			procesarRequest(request_ingresada);

			/*
			printf("%d ",request_ingresada.key);
			printf("%s ",request_ingresada.nombre_tabla);
			if(request_ingresada.header == 2){
				printf("%s",request_ingresada.value);
			}
			printf("\n");
			*/

			liberarMemoriaRequest(request_ingresada);
			sem_post(&mutexEscrituraMemoria);

			request_ingresada = recibirRequest(cliente);
		}

	}

	printf("\n");

	close(cliente);
	pthread_mutex_unlock(&mutex);
}
void enviarFS(t_request request){
	int servidor = conectarseA(IP_LOCAL, 40904);// conexion casera, tienen que ir los valores del .config
	enviarRequest(servidor,request);
	close(servidor);
}

int obtenerPuertoConfig(){
	return config_get_int_value(archivo_config,"PUERTO");
}

int obtenerTamanioMemo(){
	return config_get_int_value(archivo_config,"TAM_MEM");
}

//tipoRetardo puede ser RETARDO_GOSSIPING RETARDO_JOURNAL RETARDO_MEM RETARDO_FS
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
//se que es repetir codigo pero resulta declarativo
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

/////////////////LRU////////////////////////////////////
void agregarEnListaLRU(t_list* auxLRU,t_segmento *segment, t_pagina *page){

	if (!buscarSegmento(auxLRU,segment->path)){ //si no esta
	t_auxSegmento* nuevo = crearAuxSegmento(segment->path, page->numeroPagina);
	list_add(auxLRU, nuevo);
	} else {
		bool buscador(t_segmento* segmento){
			return buscarSegmento(auxLRU,segmento->path);
		}

		t_auxSegmento* yaUsado = list_remove_by_condition(auxLRU, (void*) buscador);
		list_add(auxLRU, yaUsado);
	}
}

t_auxSegmento* cualTengoQueSacar(t_list* auxLRU){
	return list_remove(auxLRU, 0 );
}

///////////////////////LOG/////////////////////////////////
void inicializarLogMemo(){
	FILE* logger = fopen("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/memoria/logMemoria", "r+");
    extern t_log *logMemoria;
	logMemoria = log_create(logger, program_name, is_active_console, level), "memoria.c", 0, LOG_LEVEL_DEBUG);
	return;
}


