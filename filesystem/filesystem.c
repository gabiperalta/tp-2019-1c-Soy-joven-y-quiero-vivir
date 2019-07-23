#include "filesystem.h"
#include "bibliotecaLFS.h"
#include <commons/string.h>
#include <time.h>



void gestionarFuncionFilesystem(t_request request){
	//char* valueObtenido = malloc(MAX_VALUE);
	//uint32_t timestampObtenido;
	int error;
	char* consistencia = malloc(strlen("SHC")+1);
	t_list* respuestaDescribe;
	nodo_memtable* respuestaSelect = malloc(sizeof(nodo_memtable));

	switch(request.header){
		case 1://SELECT


			respuestaSelect = selectLFS(request.nombre_tabla, string_itoa(request.key));

			if(respuestaSelect != NULL){
				log_info(FSlog, "Filesystem: \tSelect: \tTabla: %s\tTimestamp: %i Key: %i \tValue: %s",request.nombre_tabla, respuestaSelect->timestamp, respuestaSelect->key, respuestaSelect->value);
			}else{
				logError("Filesystem: Select.");
			}



			break;
		case 2://INSERT


			if(request.timestamp){

				error = insertLFS(request.nombre_tabla, string_itoa(request.key), request.value, string_itoa(request.timestamp));

			}
			else{

				error = insertLFS(request.nombre_tabla, string_itoa(request.key), request.value, "USE_TIMESTAMP");

			}

			if(!error){
				log_info(FSlog, "Filesystem: \tInsert:\tTabla: %s\tKey: %s\tValue: $s", request.nombre_tabla, request.key, request.value);
			}else{
				logError("Filesystem: Insert.");
			}


			break;
		case 3://CREATE

			switch(request.tipo_consistencia){
				case SC:
					consistencia = "SC";
					break;
				case SHC:
					consistencia = "SHC";
					break;
				case EC:
					consistencia = "EC";
				}

			error = createLFS(request.nombre_tabla, consistencia, string_itoa(request.numero_particiones), string_itoa(request.compaction_time));

			if(!error){
				log_info(FSlog, "Filesystem: Create:\tTabla: %s Tipo de Consistencia: %s\tParticiones: %s\t Tiempo de Compactacion: %s", request.nombre_tabla, request.tipo_consistencia, request.numero_particiones, request.compaction_time);
			}else{
				logError("Filesystem: Create.");
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
				logInfo("Filesystem: Describe:");
				for( int i=0; i < respuestaDescribe->elements_count; i++){
					loguearMetadata(list_get(respuestaDescribe, i));
				}
			}else{
				logError("Filesystem: Describe.");
			}


			break;
		case 5://DROP

			error = dropLSF(request.nombre_tabla);

			if(!error){
				log_info(FSlog, "Filesystem: Drop:\tTabla: %s", request.nombre_tabla);
			}else{
				logError("Filesystem: Drop");
			}

			break;
	}

	// free(valor);
	// free(consistencia);
}




int main() {

	char * linea;
	t_request request;
	fijarPuntoDeMontaje();
	inicializarLog();
	inicializarMemtable();
	inicializarBitmap();
	inicializarListaDeTablas();
	inicializarBloques();
	setearTamanioMaximoRegistro();
	setearTamanioMaximoArchivo();
	pthread_mutex_init(&mutexBitmap, NULL);
	inicializarServidorV2();
	pthread_t dumpThread;
	pthread_create(&dumpThread, NULL, (void*)dump, NULL);
	pthread_detach(dumpThread);
	inicializarHilosDeCompactacion();

	/*// PRUEBA DESCRIBE
	createLFS("TABLA1", "SC", "3", "60000");
	createLFS("TABLA2", "EC", "2", "75000");

	t_list* respuestaDescribe;

	//describeLSF("TABLA1");
	//printf("describe TABLA1");
	respuestaDescribe = describeLSF("DEFAULT");
	printf("describe DEFAULT");

	loguearMetadata(list_get(respuestaDescribe, 0));

	if(respuestaDescribe->elements_count > 0){
		logInfo("Filesystem: Describe:");
		for( int i=0; i < respuestaDescribe->elements_count; i++){
			loguearMetadata(list_get(respuestaDescribe, i));
		}
	}else{
		logError("Filesystem: Describe.");
	}
	dropLSF("TABLA1");
	dropLSF("TABLA2");

	/*createLFS("TABLA1", "SC", "3", "60000");
	{
		nodo_memtable* registro = malloc(sizeof(nodo_memtable));
		registro->key = 4;
		registro->timestamp = 100000;
		registro->value = malloc(5);
		strcpy(registro->value, "hola");

		char* direccionArchivo = direccionDeArchivo(direccionDeTabla("TABLA1"), "1.bin");
		escribirRegistroEnArchivo(direccionArchivo, registro);

	}*/


	while(1) {
		linea = readline(">"); //----- CREATE TABLA1 SC 3 60000 ----- SELECT TABLA1 4 -----	INSERT TABLA1 4 "HOLAPIPI" ----- DROP TABLA2
		if (!linea) {		   // ---------------------------------------------------------------------------------------CREATE TABLA2 SC 4 60000
			break;
		}
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}if(!strncmp(linea, "\n", 1)){
			free(linea);
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
	return 0;
}
