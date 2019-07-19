#include "filesystem.h"
#include "bibliotecaLFS.h"
#include <commons/string.h>
#include <time.h>



void gestionarFuncionFilesystem(t_request request){
	//char* valueObtenido = malloc(MAX_VALUE);
	//uint32_t timestampObtenido;
	char* valor = malloc(40);
	char* consistencia = malloc(4);
	t_list* respuestaDescribe;

	printf("hasta aca funcniona\n");
	switch(request.header){
		case 1://SELECT TABLA1 16

			valor = selectLFS(request.nombre_tabla, string_itoa(request.key));

			printf("El valor de la key %i mas nueva es %s\n", request.key, valor);

			break;
		case 2://INSERT

			printf("timestamp: %i \n", request.timestamp);
			if(request.timestamp){

				insertLFS(request.nombre_tabla, string_itoa(request.key), request.value, string_itoa(request.timestamp));

			}
			else{

				insertLFS(request.nombre_tabla, string_itoa(request.key), request.value, "USE_TIMESTAMP");

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

			createLFS(request.nombre_tabla, consistencia, string_itoa(request.numero_particiones), string_itoa(request.compaction_time));
			// TODO

			break;
		case 4://DESCRIBE

			if( request.tam_nombre_tabla > 1){

				describeLSF(request.nombre_tabla);

			}
			else{

				describeLSF("DEFAULT");

			}

			for( int i=0; i < respuestaDescribe->elements_count; i++){
				imprimirMetadata(list_get(respuestaDescribe, i));
			}

			break;
		case 5://DROP

			dropLSF(request.nombre_tabla);

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
		}

		request = gestionarSolicitud(linea);
		gestionarFuncionFilesystem(request);
		liberarMemoriaRequest(request);
		free(linea);
	}
    dictionary_destroy(diccionario);
    log_destroy(FSlog);
    bitarray_destroy(bitarray);
    pthread_mutex_destroy(&mutexBitmap);
	return 0;
}
