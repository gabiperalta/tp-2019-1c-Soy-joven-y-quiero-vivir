#include "bibliotecaLFS.h"

// PROTOTIPOS


// PERMITE LA OBTENCION DEL VALOR DE UNA KEY DENTRO DE UNA TABLA



// --------------------------------------------------- //
// -------- CONTROL DE ARCHIVOS Y DIRECTORIOS -------- //
// --------------------------------------------------- //

void crearTabla(char* nombreDeTabla, char* tipoDeConsistencia, int numeroDeParticiones, int tiempoDeCompactacion){
	int status;
	char* direccion = direccionDeTabla(nombreDeTabla);

	status = mkdir(direccion, 0777 ); //   S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
	FILE *metadata;

	if(!status){
		printf("La tabla < %s > se ha creado correctamente.\n", nombreDeTabla);

		char* direccionDeMetadata = direccionDeArchivo(direccion, "Metadata");
		metadata = fopen(direccionDeMetadata,"w");

		if(metadata){
			printf("Se creo correctamente el archivo Metadata.\n");
		}
		else{
			printf("No se creo el archivo Metadata correctamente.\n");
		}
		fprintf(metadata, "CONSISTENCY =%s \nPARTITIONS =%i \nCOMPACTION_TIME =%i", tipoDeConsistencia, numeroDeParticiones, tiempoDeCompactacion);
		fclose(metadata);
		int contador = 0;
		char* numeroDeParticion;
		char* nombreDeParticion;
		while(contador < numeroDeParticiones){
			numeroDeParticion = string_itoa(contador);
			nombreDeParticion = strcat(numeroDeParticion, ".bin");
			if(!crearArchivo(direccion, nombreDeParticion)){
				contador ++;
				printf("Se creo la particion < %i >\n", contador);
			}else{
				printf("No se creo la particion < %i >, se volvera a intentar.\n", contador);
			}
		}
	}
	else{
		printf("No se pudo crear la tabla.\n");
	}


	return;
}

t_config* devolverMetadata(char* nombreDeTabla){
	char* direccion =direccionDeArchivo(nombreDeTabla, "Metadata");
	t_config* metadata;
	metadata = config_create(direccion);
	return metadata;
}

char* direccionDeTabla(char* nombreDeTabla){
	char* direccionTables = "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/tables/"; // /home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem
	int length = strlen(direccionTables) + strlen(nombreDeTabla) + 1;
	char* direccion = malloc(length);
	strcpy(direccion, direccionTables);
	strcat(direccion, nombreDeTabla);
	return direccion;
}


char* direccionDeArchivo(char* direccionDeLaTabla, char* nombreDeArchivo){
	int length2 = strlen(direccionDeLaTabla) + strlen("/") + strlen(nombreDeArchivo) + 1;
	char* direccion = malloc(length2);
	strcpy(direccion, direccionDeLaTabla);
	strcat(direccion, "/");
	strcat(direccion, nombreDeArchivo);
	return direccion;
}

int calcularParticion(int key, int numeroDeParticiones){
	int particion = numeroDeParticiones%key;
	return particion;
}

int crearArchivo(char* direccionDeTabla, char* nombreDeArchivo){
	FILE* archivo;
	char* direccion = direccionDeArchivo(direccionDeTabla, nombreDeArchivo);
	archivo = fopen(direccion, "w");
	if(!archivo){
		return 1;
	}
	fclose(archivo);
	return 0;
}

// TERMINAR
/*void recorrerDirectorio(char* directorio){
	DIR *dip;
	            struct dirent   *dit;

	            int             contador = 0;

	            /* check to see if user entered a directory name */
	           /* if (argc < 2)
	            {
	                    printf("Usage: %s <directory>\n", argv[0]);
	                    return 0;
	            }*/

	            /* DIR *opendir(const char *name);
	             *
	             * Open a directory stream to argv[1] and make sure
	             * it's a readable and valid (directory)
	          if ((dip = opendir("src")) == NULL)
	            {
	                    error_show("opendir");
	                    return;
	            }

	            printf("Directory stream is now open\n");

	            /*  struct dirent *readdir(DIR *dir);
	             *
	             * Read in the files from argv[1] and print

	          while ((dit = readdir(dip)) != NULL)
	            {
	                    contador++;
	                    printf("\n%s", dit->d_name);
	                    printf(" %d", dit->d_fd);
	            }

	            printf("\n\nreaddir() found a total of %i files\n", contador);

	            /* int closedir(DIR *dir);
	             *
	             * Close the stream to argv[1]. And check for errors.
	            if (closedir(dip) == -1)
	            {
	                    error_show("closedir");
	                    return;
	            }

	            printf("\nDirectory stream is now closed\n");
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

// -------------- BUSQUEDA EN ARCHIVOS ----------------- //


char* escanearArchivo(char* direccionDelArchivo, int key, int esArchivoTemporal){ // si esArchivoTemporal es 1, es un .tmp, si es 0, es un .bin (se hardcodea cuando se llama a la funcion)
	FILE* archivo = fopen(direccionDelArchivo, "r");
	char* registro;
	char** registroSpliteado;
	int timestampMasGrande = 0;
	int timestampActual;
	char* registroCorrecto = malloc(100);

	char* ckey = string_itoa(key);
	if(archivo){
		do{
			while(!feof(archivo) && strcmp(registroSpliteado[1], ckey)){
				fgets(&registro, 100, archivo);
				registroSpliteado = string_split(registro, ";");
				if(!esArchivoTemporal)
					registroCorrecto = registro;
			}
			timestampActual = atoi(registroSpliteado[0]);
			if(timestampActual > timestampMasGrande){
				timestampMasGrande = timestampActual;
				registroCorrecto = registro;
			}
		}while(esArchivoTemporal);
	}
	else{
		error_show("No se pudo abrir el archivo");
	}
	fclose(archivo);
	return registroCorrecto;
}
















