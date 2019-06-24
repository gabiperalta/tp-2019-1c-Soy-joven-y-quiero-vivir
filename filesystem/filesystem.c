/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "filesystem.h"
#include "bibliotecaLFS.h"
#include <commons/string.h>
#include <time.h>



/*
int gestionarFuncionFilesystem(char* solicitud) {
	char** preSpliteado = string_split(solicitud, '"');
	char** spliteado = string_split(preSpliteado[0], ' ');


	if(!strcmp(spliteado[0], "SELECT")) {

		char* valor = selectLFS(spliteado[1], spliteado[2]);

		printf("El valor de la key %s mas nueva es %s\n", spliteado[2], valor);

	}

	else if(!strcmp(spliteado[0], "INSERT")) {

		if(cantidadElementosCharAsteriscoAsterisco(preSpliteado) == 3){

			insertLFS(spliteado[1], spliteado[2], preSpliteado[1], preSpliteado[2]);

		}
		else{

			insertLFS(spliteado[1], spliteado[2], preSpliteado[1], "USE_TIMESTAMP");

		}
	}

	else if(!strcmp(spliteado[0], "CREATE")) {

		createLFS(spliteado[1], spliteado[2], spliteado[3], spliteado[4]);

	}

	else if(!strcmp(spliteado[0], "DESCRIBE")) {

		if(cantidadElementosCharAsteriscoAsterisco(spliteado) > 1){

			describeLSF(spliteado[1]);

		}
		else{

			describeLSF("DEFAULT");

		}
	}

	else if(!strcmp(spliteado[0], "DROP")) {

		dropLSF(spliteado[1]);

	}
	else if(!strcmp(spliteado[0], "DUMP")) {

		dump();

	}

	else {

		printf("La funcion no es correcta\n");

	}

	free(spliteado);
	liberarCharAsteriscoAsterisco(preSpliteado);
	liberarCharAsteriscoAsterisco(spliteado);

	return 0;
}
*/


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

			if( request.timestamp != NULL ){

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

	free(valor);
	free(consistencia);
}




int main() {

	char * linea;
	inicializarLog();
	inicializarMemtable();
	listarDirectorio(direccionDeTabla("TABLA1"));
	inicializarBitmap();
	inicializarBloques();
	pthread_mutex_init(&mutexBitmap, NULL);
	//inicializarServidor();
	pthread_t dumpThread;
	pthread_create(&dumpThread, NULL, (void*)dump, NULL);
	pthread_detach(dumpThread);
	while(1) {
		linea = readline(">"); //----- CREATE TABLA1 SC 3 60000 ----- SELECT TABLA1 4 -----	INSERT TABLA1 4 "HOLAPIPI" ----- DROP TABLA2
		if (!linea) {		   // ---------------------------------------------------------------------------------------CREATE TABLA2 SC 4 60000
			break;
		}
		t_request request = gestionarSolicitud(linea);
		gestionarFuncionFilesystem(request);
		liberarMemoriaRequest(request);
		pthread_mutex_destroy(&mutexBitmap);
	}

	return 0;
}
