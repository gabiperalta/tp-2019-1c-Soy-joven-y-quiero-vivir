/*
 * biblioteca.c
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */


#include "biblioteca.h"



/*TAMANIODEARCHIVO
 * Retorna -1 en caso de error
 * Retorna el tamaño del archivo en caso de no haber error
 */
//int tamanioDeArchivo(FILE* f) {
//	int previo = ftell(f);
//	fseek(f, 0L, SEEK_END);
//	int tamanio = ftell(f);
//	fseek(f, previo, SEEK_SET);
//	return tamanio;
//}

t_request gestionarSolicitud(char* solicitud){
	char** spliteadoComillas = string_split(solicitud, "\"");
	char** spliteado = string_split(spliteadoComillas[0], " ");
	t_request request;

	string_to_upper(spliteado[0]);

	if(!strcmp(spliteado[0], "SELECT")){
		request.header = 1;
		request.tam_nombre_tabla = strlen(spliteado[1]) + 1;
		request.nombre_tabla = malloc(request.tam_nombre_tabla);
		strcpy(request.nombre_tabla,spliteado[1]);
		request.key = atoi(spliteado[2]);

		free(spliteado[0]);
		free(spliteado[1]);
		free(spliteado[2]);
	}

	else if(!strcmp(spliteado[0], "INSERT")){
		request.header = 2;
		request.tam_nombre_tabla = strlen(spliteado[1]) + 1;
		request.nombre_tabla = malloc(request.tam_nombre_tabla);
		strcpy(request.nombre_tabla,spliteado[1]);
		request.key = atoi(spliteado[2]);
		request.tam_value = strlen(spliteadoComillas[1]) + 1;
		request.value = malloc(request.tam_value);
		strcpy(request.value,spliteadoComillas[1]);

		free(spliteado[0]);
		free(spliteado[1]);
		free(spliteado[2]);
		free(spliteadoComillas[1]);
	}

	else if(!strcmp(spliteado[0], "CREATE")){
		request.header = 3;
		request.tam_nombre_tabla = strlen(spliteado[1]) + 1;
		request.nombre_tabla = malloc(request.tam_nombre_tabla);
		strcpy(request.nombre_tabla,spliteado[1]);
		request.tipo_consistencia = (uint8_t)obtenerTipoConsistencia(spliteado[2]);
		request.numero_particiones = atoi(spliteado[3]);
		request.compaction_time = atoi(spliteado[4]);

		free(spliteado[0]);
		free(spliteado[1]);
		free(spliteado[2]);
		free(spliteado[3]);
		free(spliteado[4]);

		//CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
	}

	else if(!strcmp(spliteado[0], "DESCRIBE")){
		request.header = 4;// fijarse bien que numero es
		request.tam_nombre_tabla = strlen(spliteado[1]) + 1;
		request.nombre_tabla = malloc(request.tam_nombre_tabla);
		strcpy(request.nombre_tabla,spliteado[1]);
		//ACA podrian pasarnos solo la palabra describe !!
	}

	else if(!strcmp(spliteado[0], "DROP")){
		request.header = 5;
		request.tam_nombre_tabla = strlen(spliteado[1]) + 1;
		request.nombre_tabla = malloc(request.tam_nombre_tabla);
		strcpy(request.nombre_tabla,spliteado[1]);
	}

	else if(!strcmp(spliteado[0], "JOURNAL")){
		request.header = 6;
	}

	else{
		printf("La función no es correcta\n");
	}

	free(spliteadoComillas[0]);
	free(spliteadoComillas);
	free(spliteado);

	return request;
}


void liberarMemoriaRequest(t_request request){
	switch(request.header){
		case SELECT:
			free(request.nombre_tabla);
			break;
		case CREATE:
			free(request.nombre_tabla);
			break;
		case DESCRIBE:
			break;
		case DROP:
			free(request.nombre_tabla);
			break;
		case INSERT:
			free(request.nombre_tabla);
			free(request.value);
			break;
		case JOURNAL:
			break;
		case ADD:
			break;
		default:
			break;
	}
}

int obtenerTipoConsistencia(char * consistencia){
	if(!strcmp(consistencia,"SC")){
		return SC;
	}
	else if(!strcmp(consistencia,"SHC")){
		return SHC;
	}
	else if(!strcmp(consistencia,"EC")){
		return EC;
	}
	else{
		return 0;
	}
}
