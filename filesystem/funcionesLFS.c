/*
 * funcionesLFS.c
 *
 *  Created on: 14 may. 2019
 *      Author: utnso
 */

#include "funcionesLFS.h"

void selectLFS(char* nombreDeTabla, char* key){
//									^--- debo cambiarlo por uint16
	int ikey = atoi(key);
	printf("El nombre ingresado es: %s\n", nombreDeTabla);
	printf("La key ingresada es: %i\n", ikey);

	DIR* dir = opendir();
	FILE* metadata;

	//	Esta operación incluye los siguientes pasos:
	//	1. Verificar que la tabla exista en el file system.
	if(dir){
		//	2. Obtener la metadata asociada a dicha tabla.
		metadata = devolverMetadata(nombreDeTabla);
		if(metadata){
			//	3. Calcular cual es la par􀆟ción que con􀆟ene dicho KEY.
			int numeroDeParticiones = config_get_int_value(metadata, "PARTITIONS");
			int  particion = calcularParticion(key, numeroDeParticiones);
			//	4. Escanear la par􀆟ción obje􀆟vo, todos los archivos temporales y la memoria temporal de dicha
			//	tabla (si existe) buscando la key deseada.
			//	5. Encontradas las entradas para dicha Key, se retorna el valor con el Timestamp más grande.
		}else{
			// ERROR: no abrio la metadata
		}



	}else if (ENOENT == errno)
	{
		/* Directory does not exist. */
	}
	else
	{
		// opendir() failed for some other reason.
	}

	/*
	if(existeLaTabla(nombreDeTabla)){
		int keyDeseada = buscarKey(nombreDeTabla, key);
		printf("El registro de la key deseada es: %s", keyDeseada);
	}
	*/
	return;
}


void insertLFS(char* nombreDeTabla, char* key, char* valor, char* timestamp){ // necesito control de errores?
	int tiempo;
	time_t resultado;
	if(!strcmp(nombreDeTabla, "USE_TIMESTAMP")){
		tiempo = time(resultado); // no se para que sirve resultado. si no lo pongo me tira error.
	}
	else{
		tiempo = atoi(timestamp);
	}
	int ikey = atoi(key);
	printf("El nombre ingresado es: %s\n", nombreDeTabla);
	printf("La key ingresada es: %i\n", ikey);
	printf("El valor ingresado es: %s\n", valor);
	printf("El timestamp ingresado es: %i\n", tiempo);
	return;
}


void createLFS(char* nombreDeTabla, char* tipoDeConsistencia, char* numeroDeParticiones, char* tiempoDeCompactacion){
	int inumeroDeParticiones = atoi(numeroDeParticiones);
	int itiempoDeCompactacion = atoi(tiempoDeCompactacion);
	printf("El nombre ingresado es: %s\n", nombreDeTabla);
	printf("El tipo de consistencia ingresada es: %s\n", tipoDeConsistencia);
	printf("El numero de particiones ingresado es: %i\n", inumeroDeParticiones);
	crearTabla(nombreDeTabla, tipoDeConsistencia, numeroDeParticiones, tiempoDeCompactacion);
	return;
}

void describeLSF(char* nombreDeTabla){
	if(!strcmp(nombreDeTabla, "DEFAULT")){
	}
	else{
	}
	printf("El nombre ingresado es: %s\n", nombreDeTabla);
	return;
}

void dropLSF(char* nombreDeTabla){
	printf("El nombre ingresado es: %s\n", nombreDeTabla);
	return;
}

