#include "bibliotecaLFS.h"

// PROTOTIPOS




// PERMITE LA OBTENCION DEL VALOR DE UNA KEY DENTRO DE UNA TABLA




// -------- CONTROL DE ARCHIVOS Y DIRECTORIOS -------- //

void crearTabla(char* nombreDeTabla, char* tipoDeConsistencia, int numeroDeParticiones, int tiempoDeCompactacion){
	int status;
	char** direccion = direccionDeTabla(nombreDeTabla);
	status = mkdir(*direccion, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	FILE *metadata;

	if(!status){
		printf("La tabla < %s > se ha creado correctamente.\n", nombreDeTabla);
	}
	else{
		printf("No se pudo crear la tabla.\n");
	}

	string_append(direccion, "Metadata");
	metadata = fopen(*direccion,"w");

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

/*bool existeLaTabla(char* nombreDeTabla){
	DIR* dir = opendir("mydir");
	if (dir)
	{
		return true;
		closedir(dir);
	}
	else if (ENOENT == errno) /* Directory does not exist.
	{
		return false;
	}
	return false;
	/*else
	{
		/* opendir() failed for some other reason.
	}
}*/

FILE* devolverMetadata(char* nombreDeTabla){
	char** direccion = direccionDeTabla(nombreDeTabla);
	FILE* metadata;
	string_append(direccion, "Metadata");
	metadata = fopen(*direccion,"r");
	return metadata;
}

char* direccionDeTabla(char* nombreDeTabla){
	char** direccion = directorioDeTablas;
	string_append(direccion, nombreDeTabla);
	return direccion;
}

int calcularParticion(int key, int numeroDeParticiones){
	int particion = key % numeroDeParciones;
	return particion;
}
























