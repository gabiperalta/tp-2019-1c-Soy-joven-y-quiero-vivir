/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "kernel.h"


//t_list* gestionarBloqueInstucciones(char** bloque, int16_t tamanioBloque) {
//
//	t_list* retorno = list_create();
//
//	for(int i=0; i<tamanioBloque; i++) {
//		BloqueInstrucciones* instruccion = malloc(strlen(bloque[i]) + sizeof(int16_t));
//
//		instruccion -> numero = i+1;
//		instruccion -> instruccion =  bloque[i];
//
//		list_add(retorno, instruccion);
//	}
//
//	return retorno;
//}

//int gestionarSolicitud(t_request solicitud) {
//
//	switch(solicitud.header) {
//		case SELECT:
//			return 0;
//			break;
//
//		case CREATE:
//			return 0;
//			break;
//
//		case DESCRIBE:
//			return 0;
//			break;
//
//		case DROP:
//			return 0;
//			break;
//
//		case INSERT:
//			return 0;
//			break;
//
//		case JOURNAL:
//			return 0;
//			break;
//
//		case ADD:
//			return 0;
//			break;
//
//		default:
//			return -1;
//			break;
//	}
//
//}


int gestionarEntradaConsola(char* solicitud) {
	char** spliteado = string_split(solicitud, " ");

	if(!laSintaxisEsCorrecta(solicitud)) {
		printf("La solicitud ingresada no es correcta\n");
		return -1;
	}

	if(!strcmp(spliteado[0], "SELECT")) {
		return 0;
	}

	else if(!strcmp(spliteado[0], "INSERT")) {
		return 0;
	}

	else if(!strcmp(spliteado[0], "CREATE")) {
		return 0;
	}

	else if(!strcmp(spliteado[0], "DESCRIBE")) {
		return 0;
	}

	else if(!strcmp(spliteado[0], "DROP")) {
		return 0;
	}

	else if(!strcmp(spliteado[0], "JOURNAL")) {
		return 0;
	}

	else if(!strcmp(spliteado[0], "ADD")) {
		return 0;
	}

	else if(!strcmp(spliteado[0], "RUN")) {
		return 0;
	}

	else if(!strcmp(spliteado[0], "METRICS")) {
		return 0;
	}

//	else {
//		printf("La función no es correcta\n");
//		return -1;
//	}
}

//t_list* gestionarInstuccion(char* linea) {
//
//	t_list* retorno = list_create();
//	BloqueInstrucciones* simple = malloc(strlen(linea) + sizeof(int16_t));
//
//	simple -> numero = 1;
//	simple -> instruccion = linea;
//
//	list_add(retorno, simple);
//
//	return retorno;
//}




int main() {

	t_queue* colaListo = queue_create();
	char * linea;

	while(1) {
		linea = readline(">");

		if (!linea) {
			break;
		}
		queue_push(colaListo, gestionarInstuccion(linea));
		free(linea);
	}



	return 0;
}

