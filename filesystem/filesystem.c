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

				horror = insertLFS(request.nombre_tabla, string_itoa(request.key), request.value, string_itoa(request.timestamp));

			}
			else{

				horror = insertLFS(request.nombre_tabla, string_itoa(request.key), request.value, "USE_TIMESTAMP");

			}


			if(!horror){
				log_info(FSlog, "Filesystem: \tInsert:\tTabla: %s\tKey: %i\tValue: $s", request.nombre_tabla, request.key, request.value);
			}else{
				logError("Filesystem: Insert.");
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
				log_info(FSlog, "Filesystem: Create:\tTabla: %s Tipo de Consistencia: %s\tParticiones: %i\t Tiempo de Compactacion: %li", request.nombre_tabla, consistencia, request.numero_particiones, request.compaction_time);
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

			horror = dropLSF(request.nombre_tabla);

			if(!horror){
				log_info(FSlog, "Filesystem: Drop:\tTabla: %s", request.nombre_tabla);
			}else{
				logError("Filesystem: Drop");
			}

			break;
	}

	free(consistencia);
	free(respuestaSelect);
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

	/*createLFS("TABLA5", "SC", "3", "5000");
	createLFS("TABLA6", "EC", "2", "5000");

	insertLFS("TABLA5", "4","HOLA","4875210");
	insertLFS("TABLA6", "1","HOLA2","4875210");
	insertLFS("TABLA5", "2","HOLA3","4875210");
	insertLFS("TABLA6", "2","HOLA4","4875210");
	insertLFS("TABLA5", "4","HOLA5","4875211");*/



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
	return 0;
}
