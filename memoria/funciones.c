/*
 * funciones.c
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#include "funciones.h"

void agregarMemoriaGossiping(){
	t_memoria* nuevo = malloc(sizeof(t_memoria));
	nuevo->ip = strdup(ip); // revisar; ahora esta bien
	nuevo->tam_ip = strlen(ip) + 1;
	nuevo->puerto = puerto_escucha_memoria;
	nuevo->id = numero_memoria;
	list_add(tabla_gossiping,nuevo);
}

void consola(){

	t_request request_ingresada;

	system("clear");
	printf("------------ MEMORIA %d ----------------\n",numero_memoria);

	char * linea;
	while(1) {
		linea = readline(">");
		if(linea)
			add_history(linea);
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}

//		pthread_t hiloSolicitudConsola;
//		pthread_create(&hiloSolicitudConsola,NULL,(void*)solicitudConsola,linea);
//		pthread_detach(hiloSolicitudConsola);

		request_ingresada = gestionarSolicitud(linea);
		procesarRequest(request_ingresada);
		liberarMemoriaRequest(request_ingresada);


		free(linea);
	}

	liberarRecursos();
}

void solicitudConsola(char* linea){
	t_request request_ingresada;

	request_ingresada = gestionarSolicitud(linea);
	procesarRequest(request_ingresada);
	liberarMemoriaRequest(request_ingresada);

	free(linea);
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

	pagina_nueva = crearPagina(obtenerIndicePagina(segmento_retornado->tabla_pagina),0,registro);
	list_add(segmento_retornado->tabla_pagina,pagina_nueva);

	agregarEnListaLRU(segmento_retornado->path,pagina_nueva);

	cantPaginasLibres--;
}

void procesoGossiping(){
	//int inicio = 1;
	int cliente;
	int indice;
	int puerto_seeds_int;
	bool activador = true;

	t_list* tabla_recibida;
	t_memoria* mem_temp; //solo para imprimir la tabla en la consola
	t_memoria* memoriaDesconectada;

	//puerto_seeds_int = atoi(puerto_seeds[0]);

	while(1){

		while (ip_seeds[indice] != NULL){

			cliente = conectarseA(ip_seeds[indice],atoi(puerto_seeds[indice]));

			if(cliente != 0){
				//printf("Si se pudo conectar\n");

				iniciarGossiping(cliente,FLAG_MEMORIA);
				tabla_recibida = recibirTablaGossiping(cliente);
				pthread_mutex_lock(&mutexTablaGossiping);
				if(list_size(tabla_recibida) != 0){
					//printf("Se recibio la tabla\n");
					obtenerUnion(tabla_gossiping,tabla_recibida);
				}
				enviarTablaGossiping(cliente,tabla_gossiping);
				pthread_mutex_unlock(&mutexTablaGossiping);

				//printf("%d\n",list_size(tabla_gossiping));

				/*
				for(int i=0; i<list_size(tabla_gossiping); i++){
					mem_temp = list_get(tabla_gossiping,i);
					printf("%d\t",mem_temp->id);
					printf("%d\t",mem_temp->tam_ip);
					printf("%s\t",mem_temp->ip);
					printf("%d\t\n",mem_temp->puerto);
				}
				*/

				close(cliente);
			}
			else{
				//printf("NO se pudo conectar\n");

				//printf("size tabla gossiping: %d\n",list_size(tabla_gossiping));

				// cambiar despues a buscarMemoriaPorIP
				pthread_mutex_lock(&mutexTablaGossiping);
				memoriaDesconectada = buscarMemoriaPorPuerto(tabla_gossiping,atoi(puerto_seeds[indice]));
				if(memoriaDesconectada != NULL){
					eliminarMemoria(tabla_gossiping,memoriaDesconectada->id);
				}
				pthread_mutex_unlock(&mutexTablaGossiping);

				/*
				for(int i=0; i<list_size(tabla_gossiping); i++){
					mem_temp = list_get(tabla_gossiping,i);
					printf("%d\t",mem_temp->id);
					printf("%d\t",mem_temp->tam_ip);
					printf("%s\t",mem_temp->ip);
					printf("%d\t\n",mem_temp->puerto);
				}
				*/
			}
			indice++;
		}

		usleep(retardo_gossiping);

		indice = 0;
	}
}

void procesoJournal(){

	while(1){

		journal();

		usleep(retardo_journal);
	}
}

t_response procesarRequest(t_request request){
	t_segmento* segmento_encontrado;
	t_segmento* segmento_nuevo;
	int posicionSegmentoNuevo;
	t_pagina* pagina_encontrada;
	t_pagina* pagina_nueva;
	char* valueObtenido = malloc(MAX_VALUE);
	t_registro registroNuevo;
	t_list* listaDescribes;

	t_response respuestaFS;
	t_response response;
	t_response* describeRecibido; // solo esta para mostrar por pantalla lo recibido
	//t_response responseRecibido;

	int servidorFS;

	switch(request.header){
		case SELECT://SELECT TABLA1 16
			usleep(retardo_acceso_memoria);
			pthread_mutex_lock(&mutexAccesoMemoria);
			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);

			if(segmento_encontrado != NULL){
				pagina_encontrada = buscarPagina(segmento_encontrado->tabla_pagina,request.key);

				if(pagina_encontrada != NULL){
					valueObtenido = obtenerValue(pagina_encontrada->direccion);
					printf("%s\n",valueObtenido);
					log_info(logMemoria, "Se ha seleccionado un value que estaba en memoria: %s",valueObtenido);
					agregarEnListaLRU(segmento_encontrado->path,pagina_encontrada);
				}
				else if(pagina_encontrada == NULL){

					//pthread_mutex_lock(&mutexMemoriaLlena);
					if(cantPaginasLibres==0){
						if(!liberarMemoriaLRU()){

							printf("MEMORIA LLENA\n");
							response.header = FULL_R;
							pthread_mutex_unlock(&mutexAccesoMemoria);
							return response;
						}
					}
					if(cantPaginasLibres > 0){
						usleep(retardo_acceso_filesystem);
						respuestaFS = solicitarFS(request);

						registroNuevo.value = respuestaFS.value;
						registroNuevo.timestamp = respuestaFS.timestamp;
						registroNuevo.key = request.key;

						pagina_nueva = crearPagina(obtenerIndicePagina(segmento_encontrado->tabla_pagina),0,registroNuevo); // bit en 0 porque el dato es consistente

						list_add(segmento_encontrado->tabla_pagina,pagina_nueva);
						log_info(logMemoria, "Se ha seleccionado un value que NO estaba en la memoria.");

						cantPaginasLibres--;

						agregarEnListaLRU(segmento_encontrado->path,pagina_nueva);

						valueObtenido = obtenerValue(pagina_nueva->direccion);

						liberarMemoriaResponse(respuestaFS);
						//pthread_mutex_unlock(&mutexMemoriaLlena);
					}

				}
			}
			else if(segmento_encontrado == NULL){

				//pthread_mutex_lock(&mutexMemoriaLlena);
				if(cantPaginasLibres==0){
					if(!liberarMemoriaLRU()){

						printf("MEMORIA LLENA\n");
						response.header = FULL_R;
						pthread_mutex_unlock(&mutexAccesoMemoria);
						return response;
					}
				}
				if(cantPaginasLibres > 0){
					usleep(retardo_acceso_filesystem);
					respuestaFS = solicitarFS(request);

					posicionSegmentoNuevo = list_add(tabla_segmentos,crearSegmento(request.nombre_tabla));
					segmento_nuevo = (t_segmento*)list_get(tabla_segmentos,posicionSegmentoNuevo);

					registroNuevo.value = respuestaFS.value;
					registroNuevo.timestamp = respuestaFS.timestamp;
					registroNuevo.key = request.key;

					pagina_nueva = crearPagina(obtenerIndicePagina(segmento_nuevo->tabla_pagina),0,registroNuevo);
					list_add(segmento_nuevo->tabla_pagina,pagina_nueva);

					log_info(logMemoria, "Se ha seleccionado un value que NO estaba en la memoria: %s",respuestaFS.value);

					cantPaginasLibres--;

					agregarEnListaLRU(segmento_encontrado->path,pagina_nueva);

					valueObtenido = obtenerValue(pagina_nueva->direccion);

					liberarMemoriaResponse(respuestaFS);
					//pthread_mutex_unlock(&mutexMemoriaLlena);
				}

			}
			pthread_mutex_unlock(&mutexAccesoMemoria);

			// if temporal solo para pruebas
			if(segmento_encontrado != NULL){
				// respuesta que se envia al kernel
				response.header = SELECT_R;
				response.tam_value = strlen(valueObtenido) + 1;
				response.value = malloc(response.tam_value);
				strcpy(response.value,valueObtenido);
				response.timestamp = 0; // al kernel no le importa el timestamp
			}

			break;
		case INSERT:
			usleep(retardo_acceso_memoria);
			pthread_mutex_lock(&mutexAccesoMemoria);
			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);
			registroNuevo.key = request.key;
			registroNuevo.value = request.value;
			registroNuevo.timestamp = getCurrentTime();

			if (segmento_encontrado!= NULL){
				pagina_encontrada = buscarPagina(segmento_encontrado->tabla_pagina,registroNuevo.key);

				if(pagina_encontrada != NULL){
					//pthread_mutex_lock(&mutexMemoriaLlena);

					uint32_t timestampObtenido = obtenerTimestamp(pagina_encontrada->direccion);

					if(timestampObtenido < registroNuevo.timestamp){//se actualiza el value
						actualizarRegistro(pagina_encontrada, registroNuevo);
						pagina_encontrada->modificado = 1;

						agregarEnListaLRU(segmento_encontrado->path,pagina_encontrada);
						log_info(logMemoria, "Se ha actualizado un value: %s",registroNuevo.value);
					}
					else if (timestampObtenido >= registroNuevo.timestamp){
						log_info(logMemoria, "No se actualizo el value %s porque tiene un timestamp menor.",registroNuevo.value);
					}
					//pthread_mutex_unlock(&mutexMemoriaLlena);
				}
				else if (pagina_encontrada == NULL){// si no la encuentra
					//pthread_mutex_lock(&mutexMemoriaLlena);

					if(cantPaginasLibres==0){
						if(!liberarMemoriaLRU()){

							printf("MEMORIA LLENA\n");
							response.header = FULL_R;
							pthread_mutex_unlock(&mutexAccesoMemoria);
							return response;
						}
					}
					if(cantPaginasLibres > 0){
						pagina_nueva = crearPagina(obtenerIndicePagina(segmento_encontrado->tabla_pagina),1,registroNuevo);
						list_add(segmento_encontrado->tabla_pagina,pagina_nueva);

						cantPaginasLibres--;

						agregarEnListaLRU(segmento_encontrado->path,pagina_nueva);
						log_info(logMemoria, "Se ha insertado un value: %s",registroNuevo.value);
						//pthread_mutex_unlock(&mutexMemoriaLlena);
					}
				}
			}
			else if (segmento_encontrado == NULL){ // no se encontro el segmento
				//pthread_mutex_lock(&mutexMemoriaLlena);
				if(cantPaginasLibres==0){
					if(!liberarMemoriaLRU()){

						printf("MEMORIA LLENA\n");
						response.header = FULL_R;
						pthread_mutex_unlock(&mutexAccesoMemoria);
						return response;
					}
				}
				if(cantPaginasLibres > 0){
					posicionSegmentoNuevo = list_add(tabla_segmentos,crearSegmento(request.nombre_tabla));
					segmento_nuevo = (t_segmento*)list_get(tabla_segmentos,posicionSegmentoNuevo);

					pagina_nueva = crearPagina(obtenerIndicePagina(segmento_nuevo->tabla_pagina),1,registroNuevo);
					list_add(segmento_nuevo->tabla_pagina,pagina_nueva);

					cantPaginasLibres--;

					agregarEnListaLRU(segmento_nuevo->path,pagina_nueva);
					log_info(logMemoria, "Se ha insertado un value: %s",registroNuevo.value);
					//pthread_mutex_unlock(&mutexMemoriaLlena);
				}
			}
			pthread_mutex_unlock(&mutexAccesoMemoria);

			printf("paginas libres: %d\n",cantPaginasLibres);

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
			listaDescribes = list_create();

			servidorFS = conectarseA(ip_fs,puerto_fs);
			respuestaFS = recibirResponse(servidorFS);

			if(respuestaFS.header == CANT_DESCRIBE_R){

				for(int i=0;i<respuestaFS.cantidad_describe; i++){
					recibirResponseDescribes(listaDescribes,servidorFS);
				}

				// solo para mostrar por pantalla
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

			response.header = CANT_DESCRIBE_R;
			response.cantidad_describe = respuestaFS.cantidad_describe;
			response.lista = listaDescribes;

			break;
		case DROP:
			usleep(retardo_acceso_memoria);
			pthread_mutex_lock(&mutexAccesoMemoria);

			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);
			if(segmento_encontrado!= NULL){
				dropSegmento(segmento_encontrado);

				log_info(logMemoria, "Se ha eliminado un segmento en memoria.");
			}
			pthread_mutex_unlock(&mutexAccesoMemoria);

			servidorFS = conectarseA(ip_fs, puerto_fs);
			enviarRequest(servidorFS,request);
			respuestaFS = recibirResponse(servidorFS);

			if(respuestaFS.header == DROP_R){
				printf("Se ha hecho el drop en FS\n");
			}
			else{
				printf("Error al hacer el drop en FS\n");
			}
			printf("%i\n", respuestaFS.header);

			close(servidorFS);

			response.header = DROP_R;
			break;
		case JOURNAL:

			journal();

			log_info(logMemoria, "Se ha hecho un journal.");
			response.header = JOURNAL_R;

			//FALTA EL JOURNAL CADA x TIEMPO
			break;
	}

	// prueba solo para imprimir
	for(int i=0; i<list_size(lista_LRU); i++){
		t_registro_LRU* registro_prueba = list_get(lista_LRU,i);
		printf("%s\t",registro_prueba->path);
		printf("%d\t",registro_prueba->numeroPagina);
		printf("%d\t\n",registro_prueba->modificado);
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
	//pthread_mutex_lock(&mutex);
	t_request request_ingresada = recibirRequest(cliente);
	t_response response_generado;
	t_list* tabla_recibida;

	while(request_ingresada.error != 1){

		if(request_ingresada.header == GOSSIPING){

			pthread_mutex_lock(&mutexTablaGossiping);
			enviarTablaGossiping(cliente, tabla_gossiping);
			//printf("Se envio la tabla\n");
			tabla_recibida = recibirTablaGossiping(cliente);
			if(list_size(tabla_recibida) != 0){
				//obtenerUnion(tabla_gossiping,tabla_recibida);
				list_clean_and_destroy_elements(tabla_gossiping,(void*) liberarMemoriaGossiping);
				for(int i=0; i<list_size(tabla_recibida); i++){
					list_add(tabla_gossiping,list_get(tabla_recibida,i));
				}
			}
			pthread_mutex_unlock(&mutexTablaGossiping);

			break;
		}
		else if(request_ingresada.header == GOSSIPING_KERNEL){

			pthread_mutex_lock(&mutexTablaGossiping);
			enviarTablaGossiping(cliente, tabla_gossiping);
			//printf("Se envio la tabla\n");
			pthread_mutex_unlock(&mutexTablaGossiping);

			break;
		}
		else{
			//procesarRequest(request_ingresada);
			do{
				if(flagFullEnviado){
					pthread_mutex_lock(&mutexMemoriaLlena);
					flagFullEnviado = 0;
				}
				response_generado = procesarRequest(request_ingresada);
			}while(response_generado.header == FULL_R && flagFullEnviado);

			/*
			printf("%d ",request_ingresada.key);
			printf("%s ",request_ingresada.nombre_tabla);
			if(request_ingresada.header == 2){
				printf("%s",request_ingresada.value);
			}
			printf("\n");
			*/

			liberarMemoriaRequest(request_ingresada);

			// se activa flag cuando se envia el mensaje de memoria llena
			if(response_generado.header == FULL_R){ flagFullEnviado = 1; }

			if(response_generado.header == CANT_DESCRIBE_R){
				t_response* describeRecibido;
				t_response structRespuesta;

				enviarCantidadDeDescribes(cliente,response_generado.cantidad_describe);

				for(int i=0; i<response_generado.cantidad_describe; i++){

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

				list_destroy(response_generado.lista);
			}
			else{
				// se envia el response generado
				enviarResponse(cliente,response_generado);
				liberarMemoriaResponse(response_generado); // cuidado
			}

			request_ingresada = recibirRequest(cliente);

			if(request_ingresada.header == JOURNAL){
				response_generado = procesarRequest(request_ingresada);
				enviarResponse(cliente,response_generado);
			}
		}

	}

	close(cliente);
}

t_response solicitarFS(t_request request){
	t_response responseFS;
	int servidor = conectarseA(ip_fs, puerto_fs);
	enviarRequest(servidor,request);
	responseFS = recibirResponse(servidor);
	close(servidor);
	return responseFS;
}

//tipoRetardo = RETARDO_GOSSIPING RETARDO_JOURNAL RETARDO_MEM RETARDO_FS
void modificarRetardos(char* tipoRetardo,int valorNuevo){
	  //SE SUPONE QUE ESTA ESTA, QUISE COPIAR LA DE LAS COMMONS Y LA VERDAD QUE MEZCLA DE T.O.d.o
	    char* nuevoRetardo;
	    snprintf(nuevoRetardo, 10, "%d", valorNuevo);
	    config_set_value(archivo_config, tipoRetardo, nuevoRetardo);
	    config_save(archivo_config);
}

char* obtenerIP(){
	return config_get_string_value(archivo_config,"IP");
}

int obtenerPuertoConfig(){
	return config_get_int_value(archivo_config,"PUERTO");
}

int obtenerTamanioMemo(){
	return config_get_int_value(archivo_config,"TAM_MEM");
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

void inicializarArchivoConfig(){
	archivo_config = config_create(PATH_CONFIG);

	ip = obtenerIP();
	puerto_escucha_memoria = obtenerPuertoConfig();
	ip_fs = obtenerIP_FS();
	puerto_fs = obtenerPuertoFS();
	ip_seeds = obtenerIP_SEEDS();
	puerto_seeds = obtenerPUERTO_SEEDS();
	numero_memoria = obtenerIdMemoria();
	tamano_memoria = obtenerTamanioMemo();
	retardo_gossiping = obtenerRetardo("RETARDO_GOSSIPING") * 1000;
	retardo_journal = obtenerRetardo("RETARDO_JOURNAL") * 1000;
	retardo_acceso_memoria = obtenerRetardo("RETARDO_MEM") * 1000;
	retardo_acceso_filesystem = obtenerRetardo("RETARDO_FS") * 1000;
}

///////////////////////LOG/////////////////////////////////

void inicializarLogMemo(){
	logMemoria = log_create(PATH_LOG,"memoria",false,LOG_LEVEL_INFO);
}

////////////////////////JOURNAL///////////////////////////////

void journal(){
	/*
	t_segmento* segmento_obtenido;
	t_pagina* pagina_obtenida;
	t_list* listaJournal = list_create();

	for(int i=0; i<list_size(tabla_segmentos); i++){
		segmento_obtenido = list_get(tabla_segmentos,i);

		for(int z=0; z<list_size(segmento_obtenido->tabla_pagina); z++){
			pagina_obtenida = list_get(segmento_obtenido->tabla_pagina,z);

			if(pagina_obtenida->modificado){
				t_request* request;
				request->header = INSERT;
				request->tam_nombre_tabla = strlen(segmento_obtenido->path) + 1;
				request->nombre_tabla = malloc(request->tam_nombre_tabla);
				strcpy(request->nombre_tabla,segmento_obtenido->path);
				request->key = obtenerKey(pagina_obtenida->direccion);
				request->timestamp = obtenerTimestamp(pagina_obtenida->direccion);
				request->tam_value = strlen(obtenerValue(pagina_obtenida->direccion)) + 1;
				request->value = malloc(request->tam_value);
				strcpy(request->value,obtenerValue(pagina_obtenida->direccion));

				list_add(listaJournal,request);
			}
		}
	}

	int servidor = conectarseA(ip_fs,puerto_fs);
	// ver si se manda cantidad de inserts antes
	enviarListaJournal(servidor,listaJournal);
	close(servidor);
	*/

	usleep(retardo_acceso_memoria);
	pthread_mutex_lock(&mutexAccesoMemoria);
	vaciarMemoria();
	list_clean_and_destroy_elements(lista_LRU,(void*) eliminarRegistroLRU);
	cantPaginasLibres = cantTotalPaginas;
	pthread_mutex_unlock(&mutexAccesoMemoria);
	pthread_mutex_unlock(&mutexMemoriaLlena);
	flagFullEnviado = 0;
}

void enviarListaJournal(int cliente, t_list* listaJournal){
	t_request* request_obtenida;

	for(int i=0; i<list_size(listaJournal); i++){
		request_obtenida = list_get(listaJournal,i);

		t_request request_journal;
		request_journal.header = request_obtenida->header;
		request_journal.tam_nombre_tabla = request_obtenida->tam_nombre_tabla;
		request_journal.nombre_tabla = malloc(request_journal.tam_nombre_tabla);
		strcpy(request_journal.nombre_tabla,request_obtenida->nombre_tabla);
		request_journal.key = request_obtenida->key;
		request_journal.timestamp = request_obtenida->timestamp;
		request_journal.tam_value = request_obtenida->tam_value;
		request_journal.value = malloc(request_journal.tam_value);

		enviarRequest(cliente,request_journal);

		liberarMemoriaRequest(request_journal);
	}
}
