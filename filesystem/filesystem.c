#include "filesystem.h"
#include "bibliotecaLFS.h"
#include <commons/string.h>
#include <time.h>



void gestionarFuncionFilesystem(t_request request){
	//char* valueObtenido = malloc(MAX_VALUE);
	//uint32_t timestampObtenido;
	int horror;
	char* consistencia = malloc(strlen("SHC")+1);
	t_list* respuestaDescribe;
	nodo_memtable* respuestaSelect;

	switch(request.header){
		case 1://SELECT


			respuestaSelect = selectLFS(request.nombre_tabla, string_itoa(request.key));


			if(respuestaSelect != NULL){
				//printf("Se hizo el select DISTINTO DE NULL\n");
				log_info(FSlog, "Filesystem: << SELECT >>:  Tabla: %s  Timestamp: %i  Key: %i  Value: %s",request.nombre_tabla, respuestaSelect->timestamp, respuestaSelect->key, respuestaSelect->value);
				free(respuestaSelect);
			}else{
				//printf("Se hizo el select IGUAL DE NULL\n");
				logError("Filesystem: << SELECT >>");
			}



			break;
		case 2://INSERT


			if(request.timestamp){

				horror = insertLFS(request.nombre_tabla, string_itoa(request.key), request.value, string_itoa(request.timestamp));

			}
			else{

				horror = insertLFS(request.nombre_tabla, string_itoa(request.key), request.value, "USE_TIMESTAMP");

			}


			if(!horror){
				log_info(FSlog, "Filesystem: << INSERT >>:  Tabla: %s  Key: %i  Value: %s", request.nombre_tabla, request.key, request.value);
			}else{
				logError("Filesystem: << INSERT >>");
			}


			break;
		case 3://CREATE

			switch(request.tipo_consistencia){
				case SC:
					strcpy(consistencia, "SC");
					break;
				case SHC:
					strcpy(consistencia, "SHC");
					break;
				case EC:
					strcpy(consistencia, "EC");
				}

			horror = createLFS(request.nombre_tabla, consistencia, string_itoa(request.numero_particiones), string_itoa(request.compaction_time));

			if(!horror){
				log_info(FSlog, "Filesystem: << CREATE >>:  Tabla: %s  Tipo de Consistencia: %s  Particiones: %i  Tiempo de Compactacion: %i", request.nombre_tabla, consistencia, request.numero_particiones, request.compaction_time);
			}else{
				logError("Filesystem: << CREATE >>");
			}

			break;
		case 4://DESCRIBE

			if( request.tam_nombre_tabla > 0){

				respuestaDescribe = describeLSF(request.nombre_tabla);

			}
			else{

				respuestaDescribe = describeLSF("DEFAULT");

			}

			if(respuestaDescribe->elements_count > 0){
				logInfo("Filesystem: << DESCRIBE >>:");
				for( int i=0; i < respuestaDescribe->elements_count; i++){
					loguearMetadata(list_get(respuestaDescribe, i));
				}
			}else{
				logError("Filesystem: << DESCRIBE >>");
			}


			break;
		case 5://DROP

			horror = dropLSF(request.nombre_tabla);

			if(!horror){
				log_info(FSlog, "Filesystem: << DROP >>: Tabla: %s", request.nombre_tabla);
			}else{
				logError("Filesystem: << DROP >>");
			}

			break;
	}

	free(consistencia);
}




int main() {

	char * linea;
	t_request request;
	fijarPuntoDeMontaje();
	inicializarLog();
	inicializarMemtable();
	inicializarBitmap();
	inicializarListaDeTablas();
	//printf("\nSe inicializo la lista de tablas\n");
	inicializarBloques();
	setearTamanioMaximoRegistro();
	setearTamanioMaximoArchivo();
	pthread_mutex_init(&mutexBitmap, NULL);
	inicializarServidorV2();
	pthread_t dumpThread;
	pthread_create(&dumpThread, NULL, (void*)dump, NULL);
	pthread_detach(dumpThread);
	inicializarHilosDeCompactacion();


	while(1) {
		linea = readline(">"); //----- CREATE TABLA1 SC 3 60000 ----- SELECT TABLA1 4 -----	INSERT TABLA1 4 "HOLAPIPI" ----- DROP TABLA2
		if (linea) {		   // ---------------------------------------------------------------------------------------CREATE TABLA2 SC 4 60000
			add_history(linea);
		}
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}

		request = gestionarSolicitud(linea);
		gestionarFuncionFilesystem(request);
		liberarMemoriaRequest(request);
		free(linea);
	}
    dictionary_destroy_and_destroy_elements(diccionario, (void*)destructorListaMemtable);
    list_destroy_and_destroy_elements(listaDeTablas, (void*)destructorDeTablaDeLista);
    log_destroy(FSlog);
    bitarray_destroy(bitarray);
    pthread_mutex_destroy(&mutexBitmap);
    //free(punto_de_montaje);

	return 0;
}
