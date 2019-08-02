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

			// debe hacer journal a todas las memorias que conozca
			enviarJournal(ip_memoria,puerto_memoria);

			break;
		case ADD: // ADD MEMORY 3 TO SC
			request = gestionarSolicitud(requestString);

			printf("Header: %d\n",request.header);
			printf("ID memoria: %d\n",request.id_memoria);
			printf("Tipo consistencia: %d\n",request.tipo_consistencia);

			agregarMemoria(request.id_memoria,request.tipo_consistencia);
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
	t_memoria* memoriaObtenida;
	t_memoria* memoriaDescribe;
	int idMemoriaAnterior = 0;
	bool activador = false;

	t_response response_recibido;

	int servidor;
	//int servidor = conectarseA(ip_memoria, puerto_memoria);// conexion casera

	while(quantumActual > 0){

		if(queue_is_empty(script)){
			break;
		}

		requestEjecutar = gestionarSolicitud(queue_pop(script));

		if(requestEjecutar.header != DESCRIBE && requestEjecutar.header != CREATE){
			printf("Hasta aca funciona\n");
			memoriaObtenida = obtenerMemoria(requestEjecutar.nombre_tabla); // obtengo ip y puerto
			printf("Hasta aca funciona\n");
		}
		else{
			memoriaDescribe = malloc(sizeof(t_memoria));
			memoriaDescribe->ip = strdup(ip_memoria); // revisar; ahora esta bien
			memoriaDescribe->tam_ip = strlen(ip_memoria) + 1;
			memoriaDescribe->puerto = puerto_memoria;
			memoriaDescribe->id = 1;
			memoriaObtenida = memoriaDescribe;
		}
		/*
		printf("%d ",requestEjecutar.key);
		printf("%s ",requestEjecutar.nombre_tabla);
		if(requestEjecutar.header == 2){
			printf("%s",requestEjecutar.value);
		}
		printf("\n");
		*/


		if(activador){
			if(idMemoriaAnterior != memoriaObtenida->id){
				close(servidor);
				servidor = conectarseA(memoriaObtenida->ip, memoriaObtenida->puerto);
				idMemoriaAnterior = memoriaObtenida->id;
			}
		}
		else{
			activador = true;
			//servidor = conectarseA(ip_memoria, puerto_memoria); // conexion predeterminada
			servidor = conectarseA(memoriaObtenida->ip, memoriaObtenida->puerto); // conexion verdadera
			idMemoriaAnterior = memoriaObtenida->id;
		}



		//servidor = conectarseA(IP_LOCAL, PUERTO_ESCUCHA_MEM);// conexion casera

		do{
			enviarRequest(servidor,requestEjecutar);

			response_recibido = recibirResponse(servidor);

			printf("%d\n",response_recibido.header);

			// parte especial para el DESCRIBE
			if(response_recibido.header == CANT_DESCRIBE_R){
				recibirMetadata(requestEjecutar.tam_nombre_tabla,response_recibido,servidor);

				// libero memoriaDescribe
				free(memoriaDescribe->ip);
				free(memoriaDescribe);
			}

			if(response_recibido.error){
				printf("No se puedo recibir la respuesta\n");
			}
			else if (response_recibido.header == SELECT_R){
				printf("%s\n",response_recibido.value);
			}
			else if (response_recibido.header == CREATE_R){
				printf("tabla creada correctamente\n");

				// libero memoriaDescribe
				free(memoriaDescribe->ip);
				free(memoriaDescribe);
			}
			else if (response_recibido.header == DROP_R){
				printf("tabla borrada correctamente\n");
			}
			else if (response_recibido.header == FULL_R){
				printf("memoria llena\n");
			}

			if(response_recibido.header == FULL_R){
				t_request requestJournal;
				requestJournal.header = JOURNAL;
				enviarRequest(servidor,requestJournal);
				response_recibido = recibirResponse(servidor);
			}
		}while(response_recibido.header == JOURNAL_R);
		//close(servidor);

		liberarMemoriaRequest(requestEjecutar);
		liberarMemoriaResponse(response_recibido);
		quantumActual--;
	}

	activador = false; // no se si es necesario ponerlo
	//close(servidor);

	//printf("\n");
	usleep(retardo_ejecucion);
	//sleep(3); // cambiarlo por el valor del config

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
	retardo_gossiping = config_get_int_value(archivo_config,"RETARDO_GOSSIPING") * 1000;
	retardo_ejecucion = config_get_int_value(archivo_config,"SLEEP_EJECUCION") * 1000;
	metadata_refresh = config_get_int_value(archivo_config,"METADATA_REFRESH") * 1000;
}

// revisar
void enviarJournal(char* ip, int puerto){
	int servidor = conectarseA(ip, puerto);
	t_request request;
	t_response response;
	request.header = JOURNAL;
	enviarRequest(servidor,request);
	response = recibirResponse(servidor);
	if(response.header == JOURNAL_R){
		log_info(archivo_log,"Journal exitoso");
	}
}

// no es el mismo de memoria
void procesoGossiping(){
	//int inicio = 1;
	int cliente;
	//char** ip_seeds;
	//char** puerto_seeds;
	//int puerto_seeds_int;
	bool activador = true;

	t_list* tabla_recibida;
	t_memoria* mem_temp; //solo para imprimir la tabla en la consola
	t_memoria* memoriaDesconectada;

	while(1){
		cliente = conectarseA(ip_memoria,puerto_memoria);

		if(cliente != 0){
			//printf("Si se pudo conectar\n");
			iniciarGossiping(cliente,FLAG_KERNEL);

			pthread_mutex_lock(&mutexTablaGossiping);
			tabla_gossiping = recibirTablaGossiping(cliente);
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
			memoriaDesconectada = buscarMemoriaPorPuerto(tabla_gossiping,puerto_memoria);

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

		usleep(retardo_gossiping);
	}
}

void agregarMemoria(int idMemoria, uint8_t tipoConsistencia){
	//int* id = malloc(sizeof(int));
	//id = idMemoria; // si algo sale mal, probar con &idMemoria
	switch(tipoConsistencia){
		case SC:
			if(list_size(criterio_SC) < 1){ // solo debe haber una memoria para SC
				pthread_mutex_lock(&mutexCriterio);
				list_add(criterio_SC,idMemoria);
				pthread_mutex_unlock(&mutexCriterio);
			}

			break;
		case SHC:
			break;
		case EC:
			pthread_mutex_lock(&mutexCriterio);
			list_add(criterio_EC,idMemoria);
			pthread_mutex_unlock(&mutexCriterio);

			break;
	}
}

t_memoria* obtenerMemoria(char* nombreTabla){
	//rand() %
	//t_tabla* tablaEncontrada;
	t_response* tablaEncontrada;
	t_memoria* memoriaObtenida;
	int idMemoriaObtenido;
	int numeroAleatorio;

	tablaEncontrada = buscarTabla(nombreTabla);

	switch(tablaEncontrada->tipo_consistencia){
		case SC:

			pthread_mutex_lock(&mutexCriterio);
			idMemoriaObtenido = list_get(criterio_SC,0);
			pthread_mutex_unlock(&mutexCriterio);

			pthread_mutex_lock(&mutexTablaGossiping);
			memoriaObtenida = buscarMemoria(tabla_gossiping,idMemoriaObtenido);
			pthread_mutex_unlock(&mutexTablaGossiping);

			break;
		case EC:

			numeroAleatorio = (rand()%list_size(criterio_EC)) + 1;

			while(ultimaMemoriaCriterioEC == memoriaObtenida){ // ultimaMemoriaCriterioEC guarda el id de la ultima memoria utilizada por EC
				numeroAleatorio = (rand()%list_size(criterio_EC)) + 1;
			}

			ultimaMemoriaCriterioEC = numeroAleatorio;

			pthread_mutex_lock(&mutexCriterio);
			idMemoriaObtenido = list_get(criterio_EC,numeroAleatorio);
			pthread_mutex_unlock(&mutexCriterio);

			pthread_mutex_lock(&mutexTablaGossiping);
			memoriaObtenida = buscarMemoria(tabla_gossiping,idMemoriaObtenido);
			pthread_mutex_unlock(&mutexTablaGossiping);

			break;
	}

	return memoriaObtenida;
}

void actualizarMetadata(){

	t_request request;
	t_response response;
	t_response* describeRecibido;
	int servidor;

	request.header = DESCRIBE;
	request.tam_nombre_tabla = 0;
	// TERMINAR
	while(1){
		enviarRequest(servidor,request);
		response = recibirResponse(servidor);

		recibirMetadata(request.tam_nombre_tabla,response,servidor);

		usleep(metadata_refresh);
	}
}

void recibirMetadata(int recibirCantidad,t_response response,int servidor){
	t_response* describeRecibido;
	t_list* lista_auxiliar;

	if(response.header == CANT_DESCRIBE_R){

		if(recibirCantidad){ // si recibo una sola tabla por DESCRIBE [NOMBRE_TABLA]
			lista_auxiliar = list_create();
			recibirResponseDescribes(lista_auxiliar,servidor);

			describeRecibido = list_get(lista_auxiliar,0);

			pthread_mutex_lock(&mutexMetadata);
			if(buscarTabla(describeRecibido->nombre_tabla) == NULL){ // si no encontro la tabla, la guarda
				list_add(metadata_tablas,describeRecibido);
			}
			pthread_mutex_unlock(&mutexMetadata);

			list_destroy(lista_auxiliar);

		}
		else{
			pthread_mutex_lock(&mutexMetadata);
			list_clean(metadata_tablas);
			for(int i=0;i<response.cantidad_describe; i++){
				recibirResponseDescribes(metadata_tablas,servidor);
			}
			pthread_mutex_unlock(&mutexMetadata);

			// solo para mostrar por pantalla
			printf("%d\n",list_size(metadata_tablas));
			for(int i=0;i<list_size(metadata_tablas); i++){
				describeRecibido = list_get(metadata_tablas,i);

				printf("%s\n",describeRecibido->nombre_tabla);
			}
		}

		log_info(archivo_log, "Se ha obtenido la metadata del FS.");

	}
	else {
		log_error(archivo_log,"Describe no recibido");
	}
}

// buscarTabla nuevo, devuelve un t_response
t_response* buscarTabla(char* nombreTabla) {
	int igualNombre(t_response* p) {
		return string_equals_ignore_case(p->nombre_tabla, nombreTabla);
	}

	return list_find(metadata_tablas, (void*) igualNombre);
}

/*
t_tabla* buscarTabla(char* nombreTabla) {
	int igualNombre(t_tabla* p) {
		return string_equals_ignore_case(p->nombre_tabla, nombreTabla);
	}

	return list_find(metadata_tablas, (void*) igualNombre);
}
*/

/*
void agregarTabla(t_response tablaRecibida) {
    t_tabla* new = malloc(sizeof(t_tabla));
    new->nombre_tabla = strdup(tablaRecibida.nombre_tabla);
    new->tam_nombre_tabla = tablaRecibida.tam_nombre_tabla;
    new->tipo_consistencia = tablaRecibida.tipo_consistencia;
    new->compaction_time = tablaRecibida.compaction_time;
    new->numero_particiones = tablaRecibida.numero_particiones;

    sem_wait(&mutexMetadata);
    list_add(metadata_tablas,new);
    sem_post(&mutexMetadata);

    //revisar, no tiene que haber una tabla repetida
}
*/
