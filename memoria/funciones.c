/*
 * funciones.c
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#include "funciones.h"

void consola(){
	/*printf("Hola\n");
int port= obtenerPuertoConfig();
	printf("%d\n",port);*///NO PUEDO PROBAR ESTO ME SALTA ERROR DE BINDEO

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
	t_pagina* pagina_nueva;
	t_registro registroNuevo;
	int servidor;

	switch(request.header){
		case 1://SELECT TABLA1 16

			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);

			if(segmento_encontrado != NULL){
				pagina_encontrada = buscarPagina(segmento_encontrado->tabla_pagina,request.key);

				if(pagina_encontrada != 0){
					valueObtenido = obtenerValue(pagina_encontrada->direccion);
					printf("%s\n",valueObtenido);
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
						}
						else if (timestampObtenido >= registroNuevo.timestamp){/*no hay que actualizar*/}
					}
					else if (pagina_encontrada == NULL){// si no la encuentra
						list_add(segmento_encontrado->tabla_pagina,crearPagina(list_size(segmento_encontrado->tabla_pagina),1,registroNuevo));

						//////////////////////
						//ver si hay espacio//
						//////////////////////
					}
				}
				else if (segmento_encontrado == NULL){ // no se encontro el segmento
					int posicionSegmentoNuevo;
					t_segmento* segmento_nuevo;

					posicionSegmentoNuevo = list_add(tabla_segmentos,crearSegmento(request.nombre_tabla));
					segmento_nuevo = (t_segmento*)list_get(tabla_segmentos,posicionSegmentoNuevo);
					list_add(segmento_nuevo->tabla_pagina,crearPagina(0,1,registroNuevo));
				}

			break;
		case 3://CREATE
		
			//enviarFS(request);
			//RECIBIR DE FS EL OK O EL ERROR
			//printf(LO_RECIBIDO);

			break;
		case 4://DESCRIBE

			//enviarFS(request);
			//RECIBIR DE FS EL OK O EL ERROR
			//printf(LO_RECIBIDO);

			break;
		case 5://DROP
			//enviarFS(request);
			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);
			if(segmento_encontrado!= NULL){
				//eliminarSegmento(segmento_encontrado);

			}

			break;
		case 6://JOURNAL
			break;
	}

	free(valueObtenido);
}


void conexionKernel(){
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

	printf("\n");

	close(cliente);
	pthread_mutex_unlock(&mutex);
}
void enviarFS(t_request request){
	servidor = conectarseA(IP_LOCAL, 40904);// conexion casera, tienen que ir los valores del .config
	enviarRequest(servidor,request);
	close(servidor);
}

int obtenerPuertoConfig(){
	return config_get_int_value(archivo_config,"PUERTO");
}

/*char* obtenerPath() {
    char buf[PATH_MAX + 1];
    char *res = realpath("memoria.config", buf);
    return res;
}// no se si hace lo que quiero
int obtenerPuertoConfig(){
	t_config* configFile;
	int puerto;
	configFile->path = "/home/utnso/tp-2019-1c-Soy-joven-y-quiero-vivir/memoria/memoria.config";
	puerto = config_get_int_value(configFile, "PUERTO=");
	return puerto;
}*/

int obtenerTamanioMemo(){
	return config_get_int_value(archivo_config,"TAM_MEM");
}

void liberarRecursos(){
	free(memoria);
	close(puerto);
	config_destroy(archivo_config);
}

