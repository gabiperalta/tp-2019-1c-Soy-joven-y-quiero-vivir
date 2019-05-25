/*
 * funcionesLFS.c
 *
 *  Created on: 14 may. 2019
 *      Author: utnso
 */

#include "funcionesLFS.h"

char* selectLFS(char* nombreDeTabla, char* key){
//									^--- debo cambiarlo por uint16
	int ikey = atoi(key);
	printf("El nombre ingresado es: %s\n", nombreDeTabla);
	printf("La key ingresada es: %i\n", ikey);
	char* valor;

	char* direccionDeLaTabla = direccionDeTabla(nombreDeTabla);
	DIR* dir = opendir(direccionDeLaTabla);
	t_config* metadata;

	//	Esta operación incluye los siguientes pasos:
	//	1. Verificar que la tabla exista en el file system.
	if(dir){
		//	2. Obtener la metadata asociada a dicha tabla.
		metadata = devolverMetadata(direccionDeLaTabla);
		if(metadata){
			//	3. Calcular cual es la par􀆟ción que con􀆟ene dicho KEY.
			// char* particiones = "PARTICIONES";
			int numeroDeParticiones = config_get_int_value(metadata, "PARTITIONS");
			int  particion = calcularParticion(ikey, numeroDeParticiones);
			//	4. Escanear la par􀆟ción obje􀆟vo, todos los archivos temporales y la memoria temporal de dicha
			//	tabla (si existe) buscando la key deseada.
			// Escanear la partción objetivo
			char* nombreDelArchivo = malloc(20);
			strcpy(nombreDelArchivo,  string_itoa(particion));
			strcat(nombreDelArchivo, ".bin");

			char* direccionDelArchivo = direccionDeArchivo(direccionDeLaTabla, nombreDelArchivo);
			char* registroBin = escanearArchivo( direccionDelArchivo, key, 0);

			char* registroTemporal = buscarEnTemporales(direccionDeLaTabla, key);

			char* registroMemtable = buscarMemoriaTemporal(nombreDeTabla, key);

			//	5. Encontradas las entradas para dicha Key, se retorna el valor con el Timestamp más grande.
			char** registroSpliteado = string_split(registroMasNuevo(registroMemtable, registroMasNuevo( registroBin, registroTemporal)), ";");

			valor = registroSpliteado[2];

		}else{
			error_show("No se abrio la metadata");
		}
	}else if (ENOENT == errno)
	{
		/* Directory does not exist. */
	}
	else
	{
		// opendir() failed for some other reason.
	}
	return valor;
}


void insertLFS(char* nombreDeTabla, char* key, char* valor, char* timestamp){ // necesito control de errores?
	int tiempo;
	time_t resultado;
	if(!string_equals_ignore_case(nombreDeTabla, "USE_TIMESTAMP")){
		tiempo = time(resultado); // no se para que sirve resultado. si no lo pongo me tira error.
	}
	else{
		tiempo = atoi(timestamp);
	}
	int ikey = atoi(key);
	extern t_dictionary *diccionario;

	printf("El nombre ingresado es: %s\n", nombreDeTabla);
	printf("La key ingresada es: %i\n", ikey);
	printf("El valor ingresado es: %s\n", valor);
	printf("El timestamp ingresado es: %i\n", tiempo);

	char* direccionDeLaTabla = direccionDeTabla(nombreDeTabla);
	DIR* dir = opendir(direccionDeLaTabla);
	t_config* metadata;


	//1. Verificar que la tabla exista en el file system. En caso que no exista, informa el error y
	//con􀆟núa su ejecución.
	if(dir){
		nodo_memtable *nuevoNodo;
		nuevoNodo->timestamp = tiempo;
		nuevoNodo->key = ikey;
		nuevoNodo->value = valor;
		//	2. Obtener la metadata asociada a dicha tabla.
		metadata = devolverMetadata(direccionDeLaTabla);
		if(metadata){

			//3. Verificar si existe en memoria una lista de datos a dumpear. De no exis􀆟r, alocar dicha
			//memoria.
			if(!dictionary_has_key(diccionario, nombreDeTabla)){
				dictionary_put(diccionario, nombreDeTabla, list_create());
			}
			//4. El parámetro Timestamp es opcional. En caso que un request no lo provea (por ejemplo
			//insertando un valor desde la consola), se usará el valor actual del Epoch UNIX.
			//5. Insertar en la memoria temporal

			// considero que aca deberia usar un semaforo
			list_add( dictionary_get(diccionario, nombreDeTabla), nuevoNodo);

		}else{
			error_show("No se abrio la metadata");
		}
	}
	else if (ENOENT == errno){
		error_show("No existe la %s en el filesystem", nombreDeTabla);
	}
	else{
		// opendir() failed for some other reason.
	}
	closedir(dir);




	return;
}


void createLFS(char* nombreDeTabla, char* tipoDeConsistencia, char* numeroDeParticiones, char* tiempoDeCompactacion){
	int inumeroDeParticiones = atoi(numeroDeParticiones);
	int itiempoDeCompactacion = atoi(tiempoDeCompactacion);
	printf("El nombre ingresado es: %s\n", nombreDeTabla);
	printf("El tipo de consistencia ingresada es: %s\n", tipoDeConsistencia);
	printf("El numero de particiones ingresado es: %i\n", inumeroDeParticiones);
	crearTabla(nombreDeTabla, tipoDeConsistencia, inumeroDeParticiones, itiempoDeCompactacion);
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

