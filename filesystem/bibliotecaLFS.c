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

t_config* devolverMetadata(char* nombreDeTabla){
	char** direccion = direccionDeTabla(nombreDeTabla);
	t_config* metadata;
	string_append(direccion, "Metadata");
	metadata = config_create(*direccion);
	return metadata;
}

char** direccionDeTabla(char* nombreDeTabla){
	char** direccion = "~/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/tables";
	string_append(direccion, nombreDeTabla);
	return direccion;
}

int calcularParticion(int key, int numeroDeParticiones){
	int particion = numeroDeParticiones%key;
	return particion;
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
}*/






















