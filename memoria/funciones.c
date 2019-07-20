/*
 * funciones.c
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#include "funciones.h"

void agregarMemoriaGossiping(){
	t_memoria* nuevo = malloc(sizeof(t_memoria));
	nuevo->ip = strdup("127.0.0.1"); // revisar
	nuevo->tam_ip = strlen(nuevo->ip) + 1;
	nuevo->puerto = obtenerPuertoConfig();
	nuevo->id = obtenerIdMemoria();
	list_add(tabla_gossiping,nuevo);
}

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
	t_pagina* pagina_nueva;
	t_registro registro;

	registro.key = 16;
	registro.timestamp = 237;
	registro.value = "Espero que funcione";

	list_add(tabla_segmentos,crearSegmento("TABLA1"));
	segmento_retornado = (t_segmento*)list_get(tabla_segmentos,0);

	pagina_nueva = crearPagina(list_size(segmento_retornado->tabla_pagina),0,registro);
	list_add(segmento_retornado->tabla_pagina,pagina_nueva);

	agregarEnListaLRU(auxLRU,segmento_retornado,pagina_nueva);
}

void procesoGossiping(){
	//int inicio = 1;
	int cliente;
	char** ip_seeds;
	char** puerto_seeds;
	int puerto_seeds_int;
	bool activador = true;

	t_list* tabla_recibida;
	t_memoria* mem_temp; //solo para imprimir la tabla en la consola
	t_memoria* memoriaDesconectada;

	ip_seeds = obtenerIP_SEEDS();
	puerto_seeds = obtenerPUERTO_SEEDS();
	puerto_seeds_int = atoi(puerto_seeds[0]);

	while(1){
		cliente = conectarseA(ip_seeds[0],puerto_seeds_int);

		if(cliente != 0){
			printf("Si se pudo conectar\n");
			iniciarGossiping(cliente);

			tabla_recibida = recibirTablaGossiping(cliente);

			if(list_size(tabla_recibida) != 0){
				printf("Se recibio la tabla\n");
				tabla_gossiping = obtenerUnion(tabla_gossiping,tabla_recibida);
			}

			printf("%d\n",list_size(tabla_gossiping));

			if(list_size(tabla_recibida) != 0){
				for(int i=0; i<list_size(tabla_gossiping); i++){
					mem_temp = list_get(tabla_gossiping,i);
					printf("%d  ",mem_temp->id);
					printf("%d  ",mem_temp->tam_ip);
					printf("%s  ",mem_temp->ip);
					printf("%d\n",mem_temp->puerto);
				}
			}

			close(cliente);
		}
		else{
			//printf("NO se pudo conectar\n");

			//printf("size tabla gossiping: %d\n",list_size(tabla_gossiping));

			// cambiar despues a buscarMemoriaPorIP
			memoriaDesconectada = buscarMemoriaPorPuerto(tabla_gossiping,puerto_seeds_int);

			if(memoriaDesconectada != NULL){
				eliminarMemoria(tabla_gossiping,memoriaDesconectada->id);
			}

			/*
			for(int i=0; i<list_size(tabla_gossiping); i++){
				mem_temp = list_get(tabla_gossiping,i);
				printf("%d  ",mem_temp->id);
				printf("%d  ",mem_temp->tam_ip);
				printf("%s  ",mem_temp->ip);
				printf("%d\n",mem_temp->puerto);
			}
			*/
//			if(activador){
//				t_memoria* nuevo = malloc(sizeof(t_memoria));
//				nuevo->ip = strdup("127.0.0.1"); // revisar
//				nuevo->tam_ip = strlen(nuevo->ip) + 1;
//				nuevo->puerto = obtenerPuertoConfig();
//				nuevo->id = obtenerIdMemoria();
//				list_add(tabla_gossiping,nuevo);
//
//				activador = 0;
//			}
		}

		sleep(3);
	}
}

void journalCadaXTiempo(){
	int tiempo = obtenerRetardo("RETARDO_JOURNAL");

	while(1){
		//int paginasNoModifcadas = cuantasNoModif(tabla_segmentos);
		//enviarCantidadDeJournal(40904,(cantTotalPaginas - cantPaginasLibres + paginasNoModificadas));
		//enviarMEMOaFS(todoMenosLoModificado);
		//vaciarMemoria(tabla_segmentos, auxLRU);
		//cantPaginasLibres= cantTotalPaginas;
		//log_info(logMemoria, "Se ha hecho un journal.");

		sleep(tiempo);
	}
}

t_response procesarRequest(t_request request){
	t_segmento* segmento_encontrado;
	t_pagina* pagina_encontrada;
	char* valueObtenido = malloc(MAX_VALUE);
	t_registro registroNuevo;
	t_list* listaDescribes = list_create();

	t_response respuestaFS;
	t_response response;
	t_response* describeRecibido; // solo esta para mostrar por pantalla lo recibido
	//t_response responseRecibido;

	int servidorFS;

	//FALTA EL COMO REEMPLAZAR LAS PAGINAS CON EL LRU

	switch(request.header){
		case SELECT://SELECT TABLA1 16
			//FALTA ~~~ mirar si esta FULL aka cantPaginasLibres = 0
			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);

			if(segmento_encontrado != NULL){
			pagina_encontrada = buscarPagina(segmento_encontrado->tabla_pagina,request.key);

			if(pagina_encontrada != 0){
				valueObtenido = obtenerValue(pagina_encontrada->direccion);
				printf("%s\n",valueObtenido);
				log_info(logMemoria, "Se ha seleccionado un value que estaba en memoria");
				//agregarEnListaLRU(auxLRU,segmento_encontrado,pagina_encontrada);
			}
			else if(pagina_encontrada == NULL){

				/*
				servidorFS = conectarseA(ip_fs, puerto_fs);
				enviarRequest(servidor,request);
				respuestaFS = recibirResponse(servidorFS);
				close(servidorFS);
				*/

				/*if(cantPaginasLibres > 0){
				//registroNuevo = crearRegistro(respuestaFS, keyNuevo); EL KEY		*/
				list_add(segmento_encontrado->tabla_pagina,crearPagina(list_size(segmento_encontrado->tabla_pagina),1,registroNuevo));
				//cantPaginasLibres--;
				//t_pagina* pagina_Nueva = buscarPagina(tabla_segmentos,request.nombre_tabla);
				//agregarEnListaLRU(auxLRU,segmento_encontrado,pagina_NUEVA);
				log_info(logMemoria, "Se ha insertado un value.");
				/*
				* }
				*/
				/*else{
				t_auxSegmento* paginaNoModificada = cualTengoQueSacar(auxLRU, tabla_segmentos);

				if(paginaNoModificada =! NULL){
				pagina_encontrada = buscarPaginaPorNumero(segmento_encontrado, paginaNoModificada->numeroPagina);
				eliminarPagina(segmento_encontrado, pagina_encontrada);
				//t_pagina* pagina_Nueva = buscarPagina(tabla_segmentos,request.nombre_tabla);
				//agregarEnListaLRU(auxLRU,segmento_encontrado,pagina_NUEVA);
				}else{
				//vaciarMemoria(tabla_segmentos, auxLRU);
				//cantPaginasLibres= cantTotalPaginas;
				log_info(logMemoria, "Se ha hecho un journal.");
				* COPIAR LO QUE DICE EN EL CASE DE JOURNAL
				* 						}

				list_add(segmento_nuevo->tabla_pagina,crearPagina(0,0,registroNuevo));
				//cantPaginasLibres--;
				//t_pagina* pagina_Nueva = buscarPagina(tabla_segmentos,request.nombre_tabla);
				//agregarEnListaLRU(auxLRU,segmento_nuevo,pagina_Nueva);
				//log_info(logMemoria, "Se ha insertado un value nuevo. ");

				}
				}else if(segmento_encontrado== NULL){
				servidorFS = conectarseA(ip_fs, puerto_fs);
				enviarRequest(servidor,request);
				respuestaFS = recibirResponse(servidorFS);

				close(servidorFS);
				int posicionSegmentoNuevo;
				t_segmento* segmento_nuevo;

				/*if(cantPaginasLibres > 0){
			//ASIGNAR REGISTRO
				//	list_add(segmento_nuevo->tabla_pagina,crearPagina(list_size(segmento_nuevo->tabla_pagina),1,registroNuevo));
				//cantPaginasLibres--;
				//t_pagina* pagina_Nueva = buscarPagina(tabla_segmentos,request.nombre_tabla);
				//agregarEnListaLRU(auxLRU,segmento_nuevo,pagina_Nueva);
				 */

				log_info(logMemoria, "Se ha seleccionado un value que NO estaba en la memoria.");
				/*
				* }*else{
				//vaciarMemoria(tabla_segmentos, auxLRU);
				//cantPaginasLibres= cantTotalPaginas;
				log_info(logMemoria, "Se ha hecho un journal.");
				COPIAR LO QUE DICE EN EL CASE DE JOURNAL
				* 						}*/
			}

			// respuesta que se envia al kernel
			response.header = SELECT_R;
			response.tam_value = strlen(valueObtenido) + 1;
			response.value = malloc(response.tam_value);
			response.timestamp = 0; // al kernel no le importa el timestamp
			strcpy(response.value,valueObtenido);

			break;
		case INSERT:
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
						//agregarEnListaLRU(auxLRU,segmento_nuevo,pagina_Encontrada);
						log_info(logMemoria, "Se ha insertado un value.");
					}
					else if (timestampObtenido >= registroNuevo.timestamp){
						log_info(logMemoria, "No se actualizo el value porque tiene un timestamp menor.");
					}
				}
				else if (pagina_encontrada == NULL){// si no la encuentra

					if(cantPaginasLibres > 0){
					list_add(segmento_encontrado->tabla_pagina,crearPagina(list_size(segmento_encontrado->tabla_pagina),1,registroNuevo));
					cantPaginasLibres--;
					t_pagina* pagina_nueva = buscarPagina(tabla_segmentos,request.nombre_tabla);
					agregarEnListaLRU(auxLRU,segmento_encontrado,pagina_nueva);
					log_info(logMemoria, "Se ha insertado un value.");

					}

					else{
					t_auxSegmento* paginaNoModificada = cualTengoQueSacar(auxLRU, tabla_segmentos);

					if(paginaNoModificada =! NULL){
					pagina_encontrada = buscarPaginaPorNumero(segmento_encontrado, paginaNoModificada->numeroPagina);
					eliminarPagina(segmento_encontrado, pagina_encontrada);
					t_pagina* pagina_nueva = buscarPagina(tabla_segmentos,request.nombre_tabla);
					agregarEnListaLRU(auxLRU,segmento_encontrado,pagina_nueva);
					}else{
					vaciarMemoria(tabla_segmentos, auxLRU);
					cantPaginasLibres= cantTotalPaginas;
					log_info(logMemoria, "Se ha hecho un journal.");
					//COPIAR LO QUE DICE EN EL CASE DE JOURNAL
											}
					}
				}
			}
			else if (segmento_encontrado == NULL){ // no se encontro el segmento
				int posicionSegmentoNuevo;
				t_segmento* segmento_nuevo;

				posicionSegmentoNuevo = list_add(tabla_segmentos,crearSegmento(request.nombre_tabla));
				segmento_nuevo = (t_segmento*)list_get(tabla_segmentos,posicionSegmentoNuevo);

				t_pagina* pagina_nueva = crearPagina(list_size(segmento_nuevo->tabla_pagina),1,registroNuevo);
				list_add(segmento_nuevo->tabla_pagina,pagina_nueva);


				if(cantPaginasLibres > 0){

					t_pagina* pagina_nueva = crearPagina(list_size(segmento_nuevo->tabla_pagina),1,registroNuevo);
					list_add(segmento_nuevo->tabla_pagina,pagina_nueva);
					cantPaginasLibres--;
					agregarEnListaLRU(auxLRU,segmento_nuevo,pagina_nueva);
					log_info(logMemoria, "Se ha insertado un value.");

				}
				else{
					vaciarMemoria(tabla_segmentos, auxLRU);
					cantPaginasLibres= cantTotalPaginas;
					log_info(logMemoria, "Se ha hecho un journal.");
					//COPIAR LO QUE DICE EN EL CASE DE JOURNAL
				}

			}

			//cantPaginasLibres--;

			response.header = INSERT_R;

			break;
		case CREATE:

			servidorFS = conectarseA(ip_fs, puerto_fs);
			enviarRequest(servidorFS,request);
			respuestaFS = recibirResponse(servidorFS);

			if(respuestaFS.header == CREATE_R){
				printf("Tabla creada\n");
			}
			else{
				printf("Error al crear tabla\n");
			}
			printf("%d\n", respuestaFS.header);

			close(servidorFS);

			log_info(logMemoria, "Se ha creado una table en el FS.");

			printf("tabla %s\n",request.nombre_tabla);
			printf("tipo consistencia %d\n",request.tipo_consistencia);
			printf("particiones %d\n",request.numero_particiones);
			printf("compaction time %d\n\n",request.compaction_time);

			response.header = CREATE_R;

			break;
		case DESCRIBE:
			//QUE KERNEL PUEDA RECIBIR LA LISTA

			servidorFS = conectarseA(ip_fs,puerto_fs);
			respuestaFS = recibirResponse(servidorFS);

			if(respuestaFS.header == CANT_DESCRIBE_R){

				for(int i=0;i<respuestaFS.cantidadDeDescribes; i++){
					recibirResponseDescribes(listaDescribes,servidorFS);
				}

				printf("%d\n",sizeof(listaDescribes));
				for(int i=0;i<sizeof(listaDescribes); i++){
					describeRecibido = list_get(listaDescribes,i);

					printf("%s\n",describeRecibido->nombre_tabla);
					//printf("%s\n",describeRecibido->nombre_tabla);
				}

				log_info(logMemoria, "Se ha obtenido la metadata del FS.");

			}
			else {
				log_error(logMemoria,"Describe no recibido");
			}

			response.header = DESCRIBE_R;
			response.lista = listaDescribes;

			break;
		case DROP:
			//FALTA: VER LAS FUNCIONES DE ADENTRO
			servidorFS = conectarseA(ip_fs, puerto_fs);
			enviarRequest(servidorFS,request);
			respuestaFS = recibirResponse(servidorFS);

			if(respuestaFS.header == CREATE_R){
				printf("Se ha hecho el drop en FS\n");
			}
			else{
				printf("Error al hacer el drop en FS\n");
			}
			printf("%i\n", respuestaFS.header);

			close(servidorFS);
			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);
			if(segmento_encontrado!= NULL){
				//eliminarSegmento(tabla_segmentos,segmento_encontrado);
				//quitarLuegoDeDrop(auxLRU,segmento_encontrado);
				//cantPaginasLibres-= saberCantidadDePaginasEliminadas(segmento_encontrado);

				log_info(logMemoria, "Se ha eliminado un segmento.");
			}
			response.header = DROP_R;
			break;
		case JOURNAL:
			//FALTA: VER LAS FUNCIONES DE ADENTRO
			//int paginasNoModifcadas = cuantasNoModif(tabla_segmentos);
			//enviarCantidadDeJournal(40904,(cantTotalPaginas - cantPaginasLibres + paginasNoModificadas));
			//enviarMEMOaFS(todoMenosLoModificado);
			//vaciarMemoria(tabla_segmentos, auxLRU);
			//cantPaginasLibres= cantTotalPaginas;
			log_info(logMemoria, "Se ha hecho un journal.");
			response.header = JOURNAL_R;

			//FALTA EL JOURNAL CADA x TIEMPO
			break;
		}
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

			enviarTablaGossiping(cliente,tabla_gossiping);
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

			if(response_generado.header == DESCRIBE_R){
				t_response* describeRecibido;
				t_response structRespuesta;

				for(int i=0; i<list_size(response_generado.lista); i++){

					describeRecibido = list_get(response_generado.lista, i);

					structRespuesta.header = DESCRIBE_R;
					structRespuesta.tam_nombre_tabla = describeRecibido->tam_nombre_tabla;
					structRespuesta.nombre_tabla = malloc(describeRecibido->tam_nombre_tabla);
					strcpy(structRespuesta.nombre_tabla, describeRecibido->nombre_tabla);
					structRespuesta.compaction_time = describeRecibido->compaction_time;
					structRespuesta.tipo_consistencia = describeRecibido->tipo_consistencia;

					enviarResponse(cliente, structRespuesta);

					free(structRespuesta.nombre_tabla);
				}
			}

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
	  //SE SUPONE QUE ESTA ESTA, QUISE COPIAR LA DE LAS COMMONS Y LA VERDAD QUE MEZCLA DE TOdo
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
///////////////////////////////////////////////////////

