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



int gestionarEntradaConsola(char* solicitud) {
	char** spliteado = string_split(solicitud, " ");


	int8_t cantidadDeElementos = cantidadElementosCharAsteriscoAsterisco(spliteado);

	if(!laSintaxisEsCorrecta(spliteado, cantidadDeElementos)) {
		printf("La solicitud ingresada no es correcta\n");
		return -1;
	}

	t_instruccion* aEnviar = malloc(sizeof(t_instruccion));
	aEnviar -> header = 0;
	aEnviar -> tam_nombre_tabla = 0;
	aEnviar -> nombre_tabla = NULL;
	aEnviar -> key = 0;
	aEnviar -> value = NULL;
	aEnviar -> consistencia = '0';
	aEnviar -> num_partciones = 0;
	aEnviar -> tiempo_compactacion = 0;
	aEnviar -> time_stamp = 0;

	if(!strcmp(spliteado[0], "SELECT")) {
		aEnviar -> header = SELECT;
		aEnviar -> key = atoi(spliteado[2]);
		aEnviar -> tam_nombre_tabla = sizeof(spliteado[1]);
		aEnviar -> nombre_tabla = malloc(sizeof(spliteado[1]));
		aEnviar -> nombre_tabla = spliteado[1];

		//agregarFuncion que le envia a la memoría
		return 0;
	}

	else if(!strcmp(spliteado[0], "INSERT")) {
		aEnviar -> header = INSERT;
		aEnviar -> key = atoi(spliteado[2]);
		aEnviar -> tam_nombre_tabla = sizeof(spliteado[1]);
		aEnviar -> nombre_tabla = malloc(sizeof(spliteado[1]));
		aEnviar -> nombre_tabla = spliteado[1];

		//agregarFuncion que le envia a la memoria

		return 0;
	}

	else if(!strcmp(spliteado[0], "CREATE") && cantidadDeElementos == 5) {//CON TIMESTAMP
		aEnviar -> header = CREATE;
		aEnviar -> key = atoi(spliteado[2]);
		aEnviar -> tam_nombre_tabla = sizeof(spliteado[1]);
		aEnviar -> nombre_tabla = malloc(sizeof(spliteado[1]));
		aEnviar -> nombre_tabla = spliteado[1];
		aEnviar -> time_stamp = atoi(spliteado[3]);

		//agregarFuncion que le envia a la memoría

		return 0;
	}

	else if(!strcmp(spliteado[0], "CREATE") && cantidadDeElementos == 4) {//SIN TIMESTAMP
			aEnviar -> header = CREATE;
			aEnviar -> key = atoi(spliteado[2]);
			aEnviar -> tam_nombre_tabla = sizeof(spliteado[1]);
			aEnviar -> nombre_tabla = malloc(sizeof(spliteado[1]));
			aEnviar -> nombre_tabla = spliteado[1];

			//agregarFuncion que le envia a la memoría

			return 0;
		}

//	else if(!strcmp(spliteado[0], "DESCRIBE")) {
//		return 0;
//	}
//
//	else if(!strcmp(spliteado[0], "DROP")) {
//		return 0;
//	}
//
//	else if(!strcmp(spliteado[0], "JOURNAL")) {
//		return 0;
//	}
//
//	else if(!strcmp(spliteado[0], "ADD")) {
//		return 0;
//	}

	else if(!strcmp(spliteado[0], "RUN")) {
		FILE* archivo = fopen("programa.lql", "r");
		char** parseado = malloc(tamanioDeArchivo(archivo) + 500);
		int16_t cantidadLineas = parsearLQL(archivo, parseado);

		for(int i=0; i<cantidadLineas; i++) {
			gestionarEntradaConsola(parseado[i]);
		}

		return 0;
	}

//	else if(!strcmp(spliteado[0], "METRICS")) {
//		return 0;
//	}
//
//	else {
//		printf("La función no es correcta\n");
//		return -1;
//	}

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

	char * linea;

	while(1) {
		linea = readline(">");

		if (!linea) {
			break;
		}
		gestionarEntradaConsola(linea);
		free(linea);
	}



	return 0;
}

