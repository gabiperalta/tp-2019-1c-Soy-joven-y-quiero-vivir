#include "bibliotecaLFS.h"

// PROTOTIPOS


// PERMITE LA OBTENCION DEL VALOR DE UNA KEY DENTRO DE UNA TABLA

void inicializarMemtable(){
	extern t_dictionary *diccionario;
	diccionario = dictionary_create();
}

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
		fprintf(metadata, "CONSISTENCY=%s \nPARTITIONS=%i \nCOMPACTION_TIME=%i", tipoDeConsistencia, numeroDeParticiones, tiempoDeCompactacion);
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

t_config* devolverMetadata(char* direccionDeLaTabla){
	char* direccion =direccionDeArchivo(direccionDeLaTabla, "Metadata");
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
	int particion = key%numeroDeParticiones;
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
int recorrerDirectorio(char* direccionDirectorio) {
    DIR *directorio;
            struct dirent   *stream;

            int contador = 0;

          if ((directorio = opendir(direccionDirectorio)) == NULL)
            {
                    error_show("No se pudo abrir el directorio.\n");
                    return 0;
            }

            printf("Se abrio el directorio correctamente.\n");

          while ((stream = readdir(directorio)) != NULL)
            {
        	  	  if(elArchivoEsDelTipo(stream->d_name, ".tmp"))
        	  	  	contador++;
                    printf("\n El nombre es:%s\n", stream->d_name);
            }

            printf("\n\nSe encotro un total de %i archivos\n", contador);

            if (closedir(directorio) == -1)
            {
                    error_show("No se pudo cerrar el directorio\n");
                    return 0;
            }

            printf("\nSe cerro correctamente el directorio\n");
    return contador;
}

int elArchivoEsDelTipo(char* archivo, char* tipoQueDebeSer){
	return string_ends_with(archivo, tipoQueDebeSer);
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


char* escanearArchivo(char* direccionDelArchivo, char* key, int esArchivoTemporal){ // si esArchivoTemporal es 1, es un .tmp, si es 0, es un .bin (se hardcodea cuando se llama a la funcion)
	FILE* archivo = fopen(direccionDelArchivo, "r");
	char* registro = malloc(100);
	char** registroSpliteado;
	char* registroCorrecto = malloc(100);
	strcpy(registroCorrecto, "N");

	if(archivo){
		do{
			do{
				fgets(registro, 100, archivo);
				registroSpliteado = string_split(registro, ";");
				if(!esArchivoTemporal)
					strcpy(registroCorrecto, registro);;
			}while(!feof(archivo) && strcmp(registroSpliteado[1], key));
			if(esArchivoTemporal){
				strcpy(registroCorrecto, registroMasNuevo( registroCorrecto, registro));
			}
		}while(esArchivoTemporal && !feof(archivo));
	}
	else{
		error_show("No se pudo abrir el archivo");
	}

	free(registro);

	fclose(archivo);
	return registroCorrecto;
}

char* buscarEnTemporales(char* direccionDeLaTabla,char* key){
	char* registroCorrecto = malloc(100);
	char* registroActual = malloc(100);
	int cantidadDeTemporales = recorrerDirectorio(direccionDeLaTabla);
	char* nombreDelArchivo = malloc(20);
	int temporalActual = 0;
	char* direccionDelArchivo = malloc(120);
	//registroCorrecto = NULL;
	strcpy(registroCorrecto, "N");		// Le pongo N para saber cunado el 'registroCorrecto' no ha sido cargado

	while(temporalActual < cantidadDeTemporales){
		strcpy(nombreDelArchivo,  string_itoa(temporalActual));
		strcat(nombreDelArchivo, ".tmp");

		direccionDelArchivo = direccionDeArchivo(direccionDeLaTabla, nombreDelArchivo);
		registroActual = escanearArchivo(direccionDelArchivo, key, 1);

		registroCorrecto = registroMasNuevo(registroCorrecto, registroActual);
		temporalActual ++;
	}
	return registroCorrecto;
}

char* buscarMemoriaTemporal(char* nombreDeTabla, char* key){
	extern t_dictionary *diccionario;
	char* registroFinal = malloc(100);
	// strcpy(registroFinal, "N");
	if(!dictionary_has_key(diccionario, nombreDeTabla)){
		return "N";
	}
	t_list *listaMemTable = dictionary_get(diccionario, nombreDeTabla);
	nodo_memtable *registro;
	nodo_memtable *registroCorrecto;
	registroCorrecto->timestamp = 0;

	int indice = 0;

	while(indice < listaMemTable->elements_count){
			registro = list_get(listaMemTable, indice);
		if(registro->timestamp > registroCorrecto->timestamp){
			registroCorrecto->timestamp = registro->timestamp;
			registroCorrecto->key = registro->key;
			registroCorrecto->value = registro->value;
		}
		indice ++;
	}

	strcpy(registroFinal, string_itoa(registroCorrecto->timestamp));
	strcat(registroFinal, ";");
	strcat(registroFinal, string_itoa(registroCorrecto->key));
	strcat(registroFinal, ";");
	strcat(registroFinal, registroCorrecto->value);

	return registroFinal;
}

char* registroMasNuevo(char* primerRegistro, char* segundoRegistro){
	if(strcmp(primerRegistro, "N")){ //   strcmp(primerRegistro, "N")    		primerRegistro != NULL
		char** primerRegistroSpliteado = string_split(primerRegistro, ";");
		char** segundoRegistroSpliteado = string_split(segundoRegistro, ";");

		int timestampprimerRegistro = atoi(primerRegistroSpliteado[0]);
		int timestampSegundoRegistro = atoi(segundoRegistroSpliteado[0]);

		if(timestampprimerRegistro >= timestampSegundoRegistro)
			return primerRegistro;
	}
	return segundoRegistro;
}















