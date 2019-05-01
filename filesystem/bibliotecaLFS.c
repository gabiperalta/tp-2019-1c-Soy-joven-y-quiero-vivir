#include "bibliotecaLFS.h"


// PERMITE LA OBTENCION DEL VALOR DE UNA KEY DENTRO DE UNA TABLA
void selectLFS(char* nombreDeTabla, char* key){
//									^--- debo cambiarlo por uint16
	int ikey = atoi(key);
	printf("El nombre ingresado es: %s\n", nombreDeTabla);
	printf("La key ingresada es: %i\n", ikey);
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



// -------- CONTROL DE ARCHIVOS Y DIRECTORIOS -------- //

void crearTabla(char* nombreDeTabla, char* tipoDeConsistencia, int numeroDeParticiones, int tiempoDeCompactacion){
	int status;
	char* direccion = "~/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/tables";
	strcat(direccion, nombreDeTabla);
	status = mkdir(direccion, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	FILE *metadata;

	if(!status){
		printf("La tabla < %s > se ha creado correctamente.\n", nombreDeTabla);
	}
	else{
		printf("No se pudo crear la tabla.\n");
	}

	strcat(direccion, "Metadata");
	metadata = fopen(direccion,"w");

	if(metadata){
		printf("Se creo correctamente el archivo Metadata.\n");
	}
	else{
		printf("No se creo el archivo Metadata correctamente.\n");
	}
	fprintf(metadata, "CONSISTENCY =%s \nPARTITIONS =%i \nCOMPACTION_TIME =%i", tipoDeConsistencia, numeroDeParticiones, tiempoDeCompactacion);
	fclose(metadata);
	return;
}























