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

//int ejecutarSQL(char* rutaSQL) {
//	FILE* archivo = fopen(rutaSQL, "r");
//
//	if(archivo == NULL) {
//		return -1;
//	}
//
//	char** parseado = malloc(tamanioDeArchivo(archivo) + 500);
//	int16_t cantidadLineas = parsearLQL(archivo, parseado);
//
//	for(int i=0; i<cantidadLineas; i++) {
//		gestionarEntradaConsola(parseado[i]);
//		free(parseado[i]);
//	}
//
//	free(parseado);
//	fclose(archivo);
//
//	return 0;
//}

int gestionarEntradaConsola(char* solicitud, int puerto, char* ip) {
	char** spliteado = string_split(solicitud, " ");


	int8_t cantidadDeElementos = cantidadElementosCharAsteriscoAsterisco(spliteado);

	if(!laSintaxisEsCorrecta(spliteado, cantidadDeElementos)) {
		printf("La solicitud ingresada no es correcta\n");
		return -1;
	}



	if(!strcmp(spliteado[0], "RUN")) {
		//ejecutarSQL(spliteado[1]);
		FILE* archivo = fopen(spliteado[1], "r");

			if(archivo == NULL) {
				return -1;
			}

			char** parseado = malloc(tamanioDeArchivo(archivo) + 500);
			int16_t cantidadLineas = parsearLQL(archivo, parseado);

			for(int i=0; i<cantidadLineas; i++) {
				gestionarEntradaConsola(parseado[i], puerto, ip);
				free(parseado[i]);
			}

			free(parseado);
			fclose(archivo);

		return 0;
	}

	else {

		int socketEnvio = conectarseA(ip, puerto);
		t_request auxiliarEnvio; //ERROR LOCO
		auxiliarEnvio = gestionarSolicitud(solicitud);
		enviarRequest(socketEnvio, auxiliarEnvio);

		//HELP

	}






	printf("No se produjo un error en el momento adecuado");
	return -1;
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

	t_config* configuracion;
	configuracion = config_create("Config.bin");

	int puerto = config_get_int_value(configuracion, "PUERTO_MEMORIA");
	char* ip = malloc(50);
	strcpy(ip, config_get_string_value(configuracion, "IP_MEMORIA"));


	char * linea;

	while(1) {
		linea = readline(">");

		if (!linea) {
			break;
		}
		gestionarEntradaConsola(linea, puerto, ip);
		free(linea);
	}



	return 0;
}

