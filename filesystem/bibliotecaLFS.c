#include "bibliotecaLFS.h"

// PROTOTIPOS


// PERMITE LA OBTENCION DEL VALOR DE UNA KEY DENTRO DE UNA TABLA

void inicializarMemtable(){
	extern t_dictionary *diccionario;
	diccionario = dictionary_create();
}

t_config* obtenerConfigDeFS(){
	t_config* config = config_create("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Config.bin");
	return config;
}

uint32_t getCurrentTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
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
		free(direccionDeMetadata);
		if(metadata){
			printf("Se creo correctamente el archivo Metadata.\n");
		}
		else{
			printf("No se creo el archivo Metadata correctamente.\n");
		}
		fprintf(metadata, "CONSISTENCY=%s \nPARTITIONS=%i \nCOMPACTION_TIME=%i", tipoDeConsistencia, numeroDeParticiones, tiempoDeCompactacion);
		fclose(metadata);
		uint8_t contador = 0;
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
	free(direccion);

	return;
}

t_config* devolverMetadata(char* direccionDeLaTabla){
	char* direccion =direccionDeArchivo(direccionDeLaTabla, "Metadata");
	t_config* metadata;
	metadata = config_create(direccion);
	free(direccion);
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
	uint8_t particion = key%numeroDeParticiones;
	return particion;
}

int crearArchivo(char* direccionDeTabla, char* nombreDeArchivo){
	FILE* archivo;
	char* direccion = direccionDeArchivo(direccionDeTabla, nombreDeArchivo);
	archivo = fopen(direccion, "w");
	free(direccion);
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

            uint8_t contador = 0;

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

char** listarDirectorio(char* direccionDirectorio){
	DIR *directorio;
			struct dirent   *stream;

			uint8_t contador = 0;

		  if ((directorio = opendir(direccionDirectorio)) == NULL)
			{
					error_show("No se pudo abrir el directorio.\n");
					return 0;
			}

			printf("Se abrio el directorio correctamente.\n");
			while ((stream = readdir(directorio)) != NULL)
			{
			  	  if ( (strcmp(stream->d_name, ".")!=0) && (strcmp(stream->d_name, "..")!=0) )
			  		  contador++;
			}

			rewinddir(directorio);

			char** array = malloc(contador * sizeof(char*));
			int indice = 0;
			while ((stream = readdir(directorio)) != NULL)
			{
				if ( (strcmp(stream->d_name, ".")!=0) && (strcmp(stream->d_name, "..")!=0) ){
				array[indice] = stream->d_name;
				indice++;
				printf("\n El nombre es:%s\n", stream->d_name);
				}
			}

			printf("\n\nSe encotro un total de %i archivos\n", contador);
			printf("\n a continuacion se listaran los archivos identificados:\n");
			for(indice = 0; indice < contador; indice++){
				printf("%s\n", array[indice]);
			}


			if (closedir(directorio) == -1)
			{
					error_show("No se pudo cerrar el directorio\n");
					return NULL;
			}
			printf("\nSe cerro correctamente el directorio\n");

	return array;
}


int elArchivoEsDelTipo(char* archivo, char* tipoQueDebeSer){
	return string_ends_with(archivo, tipoQueDebeSer);
}


void dump(){
	t_config* metadata = obtenerConfigDeFS();
	int tiempoDeDumpeo = config_get_int_value(metadata, "TIEMPO_DUMP");
	config_destroy(metadata);
	extern t_dictionary *diccionario;

	while(1){
	sleep(tiempoDeDumpeo/1000);
	printf("Se hizo un DUMP\n");

	if(!dictionary_is_empty(diccionario)){
	dictionary_iterator(diccionario, (void*)pasarAArchivoTemporal);
	dictionary_clean_and_destroy_elements(diccionario, (void*)list_destroy);

	}
	else{
		printf("Se intento hacer un DUMP pero la memoria temporal esta vacia.\n");
	}
	// listmap()
	}
	return;
}


void pasarAArchivoTemporal(char* nombreDeTabla, t_list* registros){
	char* direccion = direccionDeTabla(nombreDeTabla);
	uint8_t numeroDeTemporal = recorrerDirectorio(direccion);
	char* nombreDeArchivo = malloc(6);
	strcpy(nombreDeArchivo, string_itoa(numeroDeTemporal));
	strcat(nombreDeArchivo, ".tmp");
	char* direccionArchivo = direccionDeArchivo(direccion, nombreDeArchivo);
	free(direccion);
	FILE* archivo = fopen(direccionArchivo, "w");
	uint8_t posicion = registros->elements_count - 1;
	nodo_memtable *unRegistro;
	char* stringRegistro;
	free(direccionArchivo);
	while(posicion >= 0){
		unRegistro = list_remove( registros, posicion);
		stringRegistro = pasarRegistroAString(unRegistro);
		fprintf(archivo, "%s\n", stringRegistro);
		printf("Se guardo el registro < %s > en el archivo < %s >", stringRegistro, nombreDeArchivo);
		// seguir
		posicion --;
	}
	fclose(archivo);

	return;
}


// ----------------------------------------------------- //
// -------------- BUSQUEDA EN ARCHIVOS ----------------- //
// ----------------------------------------------------- //

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
	liberarCharAsteriscoAsterisco(registroSpliteado);

	fclose(archivo);
	return registroCorrecto;
}

char* buscarEnTemporales(char* direccionDeLaTabla,char* key){
	char* registroCorrecto = malloc(100); // cambiar por el mayor tamano posible de un registro
	//char* registroActual = malloc(100);
	uint8_t cantidadDeTemporales = recorrerDirectorio(direccionDeLaTabla);
	char* nombreDelArchivo = malloc(20);
	uint8_t temporalActual = 0;
	//char* direccionDelArchivo = malloc(sizeof(direccionDeLaTabla) + 7); // 7 seria el tamanio estimado del nombre de un n.tmp
	//registroCorrecto = NULL;
	strcpy(registroCorrecto, "N");		// Le pongo N para saber cunado el 'registroCorrecto' no ha sido cargado

	while(temporalActual < cantidadDeTemporales){
		strcpy(nombreDelArchivo,  string_itoa(temporalActual));
		strcat(nombreDelArchivo, ".tmp");

		char* direccionDelArchivo = direccionDeArchivo(direccionDeLaTabla, nombreDelArchivo);
		char* registroActual = escanearArchivo(direccionDelArchivo, key, 1);

		registroCorrecto = registroMasNuevo(registroCorrecto, registroActual);
		temporalActual ++;
		free(registroActual);
		free(direccionDelArchivo);
	}

	free(nombreDelArchivo);
	//free(direccionDelArchivo);

	return registroCorrecto;
}

char* buscarMemoriaTemporal(char* nombreDeTabla, char* key){
	extern t_dictionary *diccionario;
	//char* registroFinal = malloc(100);
	if(!dictionary_has_key(diccionario, nombreDeTabla))
		return "N";

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

	if(registroCorrecto->timestamp == 0)
		return "N";

	char* registroFinal = pasarRegistroAString( registroCorrecto);

	return registroFinal;
}

char* pasarRegistroAString(nodo_memtable* registro){
	char* registroFinal = malloc(100);
	strcpy(registroFinal, string_itoa(registro->timestamp));
	strcat(registroFinal, ";");
	strcat(registroFinal, string_itoa(registro->key));
	strcat(registroFinal, ";");
	strcat(registroFinal, registro->value);
	return registroFinal;
}

char* registroMasNuevo(char* primerRegistro, char* segundoRegistro){
	if(primerRegistro[0] == 'N'){ //   strcmp(primerRegistro, "N")    		primerRegistro != NULL
		char** primerRegistroSpliteado = string_split(primerRegistro, ";");
		char** segundoRegistroSpliteado = string_split(segundoRegistro, ";");

		int timestampprimerRegistro = atoi(primerRegistroSpliteado[0]);
		int timestampSegundoRegistro = atoi(segundoRegistroSpliteado[0]);

		liberarCharAsteriscoAsterisco(primerRegistroSpliteado);
		liberarCharAsteriscoAsterisco(segundoRegistroSpliteado);

		if(timestampprimerRegistro >= timestampSegundoRegistro)
			return primerRegistro;
	}
	free(primerRegistro);
	return segundoRegistro;
}

uint8_t cantidadElementosCharAsteriscoAsterisco(char** array){
	uint8_t size;
	for(size = 0; array[size] != NULL; size++);
	return size;
}

void liberarCharAsteriscoAsterisco(char** array){
	uint8_t size = cantidadElementosCharAsteriscoAsterisco(array);
	for(int i = 0; i < size; i++){
		free(array[i]);
	}
	free(array);
	return;
}


void eliminarTabla(char* nombreDeTabla){

	char* direccion = direccionDeTabla(nombreDeTabla);

   size_t path_len;
   char *full_path;
   DIR *dir;
   struct stat stat_path, stat_entry;
   struct dirent *entry;

   // stat for the path
   stat(direccion, &stat_path);

   // if path does not exists or is not dir - exit with status -1
   if (S_ISDIR(stat_path.st_mode) == 0) {
	   fprintf(stderr, "%s: %s\n", "Is not directory", direccion);
	   exit(-1);
   }

   // if not possible to read the directory for this user
   if ((dir = opendir(direccion)) == NULL) {
	   fprintf(stderr, "%s: %s\n", "Can`t open directory", direccion);
	   exit(-1);
   }

   // the length of the path
   path_len = strlen(direccion);;

   // iteration through entries in the directory
   while ((entry = readdir(dir)) != NULL) {

	   // skip entries "." and ".."
	   if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
		   continue;

	   // determinate a full path of an entry
	   full_path = calloc(path_len + strlen(entry->d_name) + 1, sizeof(char));
	   strcpy(full_path, direccion);
	   strcat(full_path, "/");
	   strcat(full_path, entry->d_name);

	   // stat for the entry
	   stat(full_path, &stat_entry);

	   // recursively remove a nested directory
	   if (S_ISDIR(stat_entry.st_mode) != 0) {
		   eliminarTabla(full_path);
		   continue;
	   }

	   // remove a file object
	   if (unlink(full_path) == 0)
		   printf("Removed a file: %s\n", full_path);
	   else
		   printf("Can`t remove a file: %s\n", full_path);
   }

   // remove the devastated directory and close the object of it
   if (rmdir(direccion) == 0)
	   printf("Removed a directory: %s\n", direccion);
   else
	   printf("Can`t remove a directory: %s\n", direccion);

   free(direccion);
   closedir(dir);

   return;
}






