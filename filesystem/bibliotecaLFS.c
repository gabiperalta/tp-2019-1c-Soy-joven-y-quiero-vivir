#include "bibliotecaLFS.h"

// PROTOTIPOS


// PERMITE LA OBTENCION DEL VALOR DE UNA KEY DENTRO DE UNA TABLA

void inicializarMemtable(){
	extern t_dictionary *diccionario;
	diccionario = dictionary_create();
	return;
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

int existeLaTabla(char* nombreDeTabla){
	char* direccionDeLaTabla = direccionDeTabla(nombreDeTabla);
	DIR* dir = opendir(direccionDeLaTabla);
	return dir;
}

void crearTabla(char* nombreDeTabla, char* tipoDeConsistencia, int numeroDeParticiones, int tiempoDeCompactacion){
	int status;
	char* direccion = direccionDeTabla(nombreDeTabla);

	status = mkdir(direccion, 0777 ); //   S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
	FILE *metadata;

	if(!status){
		printf("La tabla < %s > se ha creado correctamente.\n", nombreDeTabla);

		char* direccionDeMetadata = direccionDeArchivo(direccion, "Metadata");
		metadata = fopen(direccionDeMetadata,"w");
		//free(direccionDeMetadata);
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
			crearArchivoPuntoBin(direccion, nombreDeParticion);
			/*if(!crearArchivoPuntoBin(direccion, nombreDeParticion)){
				contador ++;
				printf("Se creo la particion < %i >\n", contador);
			}else{
				printf("No se creo la particion < %i >, se volvera a intentar.\n", contador);
			}*/
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

void imprimirMetadata(datos_metadata* datosDeMetadata){
	printf("\nMetadata de < %s >:\n\n", datosDeMetadata->nombreTabla);
	printf("CONSISTENCY = %s \n "
			"PARTITIONS = %i \n"
			"COMPACTION_TIME = %i \n\n", datosDeMetadata->consistencia, datosDeMetadata->particiones, datosDeMetadata->tiempoDeCompactacion);
	return;
}





char* direccionDeTabla(char* nombreDeTabla){
	//char* direccionTables = "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/tables/"; // /home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem
	int length = strlen(DIRECCION_TABLAS) + strlen(nombreDeTabla) + 1;
	char* direccion = malloc(length);
	strcpy(direccion, DIRECCION_TABLAS);
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

t_list* listarDirectorio(char* direccionDirectorio){
	DIR *directorio;
	t_list* listaDeArchivos = list_create();
			struct dirent   *stream;

		  if ((directorio = opendir(direccionDirectorio)) == NULL)
			{
					error_show("No se pudo abrir el directorio.\n");
					return NULL;
			}

			printf("Se abrio el directorio correctamente.\n");
			/*while ((stream = readdir(directorio)) != NULL)
			{
			  	  if ( (strcmp(stream->d_name, ".")!=0) && (strcmp(stream->d_name, "..")!=0) )
			  		  contador++;
			}*/

			//rewinddir(directorio);

			//char** array = malloc(contador * sizeof(char*));
			//int indice = 0;
			while ((stream = readdir(directorio)) != NULL)
			{
				if ( (strcmp(stream->d_name, ".")!=0) && (strcmp(stream->d_name, "..")!=0) ){
					list_add(listaDeArchivos, stream->d_name);
				//array[indice] = stream->d_name;
				//indice++;
						printf("\n El nombre es:%s\n", stream->d_name);
				}
			}

			//printf("\n\nSe encotro un total de %i archivos\n", contador);
			printf("\n a continuacion se listaran los archivos identificados:\n");
			/*for(indice = 0; indice < contador; indice++){
				printf("%s\n", array[indice]);
			}*/
			char* nombreArchivo;
			for(int i=0; i<listaDeArchivos->elements_count; i++){
				nombreArchivo = list_get(listaDeArchivos, i);
				printf("%s\n", nombreArchivo);
			}


			if (closedir(directorio) == -1)
			{
					error_show("No se pudo cerrar el directorio\n");
					return NULL;
			}
			printf("\nSe cerro correctamente el directorio\n");

	return listaDeArchivos;
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
	//free(direccion);
	//FILE* archivo = fopen(direccionArchivo, "w");
	uint8_t posicion = registros->elements_count - 1;
	nodo_memtable *unRegistro;
	char* stringRegistro;
	//free(direccionArchivo);
	while(posicion >= 0){
		unRegistro = list_remove( registros, posicion);
		//stringRegistro = pasarRegistroAString(unRegistro);

		escribirRegistroEnArchivo(direccionArchivo, unRegistro);

		//fprintf(archivo, "%s\n", stringRegistro);
		printf("Se guardo el registro < %s > en el archivo < %s >", stringRegistro, nombreDeArchivo);
		// seguir
		posicion --;
	}
	//fclose(archivo);
	free(direccion);
	free(direccionArchivo);
	return;
}


// ----------------------------------------------------- //
// -------------- BUSQUEDA EN ARCHIVOS ----------------- //
// ----------------------------------------------------- //

nodo_memtable* escanearArchivo(char* direccionDelArchivo, char* key, int esArchivoTemporal){ // si esArchivoTemporal es 1, es un .tmp, si es 0, es un .bin (se hardcodea cuando se llama a la funcion)
	t_config* archivo = config_create(direccionDelArchivo);
	char** bloques = config_get_array_value(archivo, "BLOCKS");
	//nodo_memtable* registro;
	//char* unRegistro = malloc(tamanioMaximoDeRegistro);
	//char** registroSpliteado;
	nodo_memtable* registroCorrecto;
	//strcpy(registroCorrecto->value, "");  // PUEDE GENERAR ERROR POR NO ASIGNARLE MEMORIA

	registroCorrecto = buscarRegistroMasNuevo(bloques, key, esArchivoTemporal);

	/*if(archivo){
		do{
			do{
				fgets(registro, tamanioMaximoDeRegistro, archivo);
				registroSpliteado = string_split(registro, ";");
				if(!esArchivoTemporal){
					//strcpy(registroCorrecto, registro);
					deRegistroSpliteadoANodoMemtable(registroSpliteado, registroCorrecto);
				}

			}while(!feof(archivo) && strcmp(registroSpliteado[1], key));
			if(esArchivoTemporal){

				registroCorrecto = registroMasNuevo(registroCorrecto, registro);
				//strcpy(registroCorrecto, registroMasNuevo( registroCorrecto, registro));

			}
		}while(esArchivoTemporal && !feof(archivo));
	}
	else{
		error_show("No se pudo abrir el archivo");
	}

	//free(registro);
	//liberarCharAsteriscoAsterisco(registroSpliteado);

	fclose(archivo);
	free(registro);*/
	return registroCorrecto;
}

nodo_memtable* buscarRegistroMasNuevo(char** listaDeBloques, char* key, int esArchivoTemporal){
	int lengthListaDeBloques = cantidadElementosCharAsteriscoAsterisco(listaDeBloques);
	char* registroIncompleto = malloc(tamanioMaximoDeRegistro);
	char* unRegistro = malloc(tamanioMaximoDeRegistro);
	bool completo = true;
	nodo_memtable* registroCorrecto;
	nodo_memtable* registro;
	char** registroSpliteado;
	strcpy(registroCorrecto->value, "");  // PUEDE GENERAR ERROR POR NO ASIGNARLE MEMORIA
	int i = 0;
	bool registroNuevo = false;

	//for(int i=0;i<lengthListaDeBloques;i++)   ACA ME QUEDE
	do{
		char* direccionDelBloque = direccionDeBloque(listaDeBloques[i]);
		FILE* archivo = fopen(direccionDelBloque, "r");

		if(archivo){
			do{
				fgets(unRegistro, tamanioMaximoDeRegistro, archivo);
				if(!completo){
					strcat(registroIncompleto, unRegistro);
					strcpy(unRegistro, registroIncompleto);
				}
				if(registroCompleto(unRegistro)){
					registroSpliteado = string_split(unRegistro, ';');
					if(strcmp(registroSpliteado[1], key)){
						if(!string_is_empty(registroCorrecto->value)){
							registroNuevo = true;
						}else{
							deRegistroSpliteadoANodoMemtable(registroSpliteado, registro);
						}
						deRegistroSpliteadoANodoMemtable(registroSpliteado, registroCorrecto);
					}
					completo = true;
				}
				else{
					strcpy(registroIncompleto, unRegistro);
					completo = false;
				}
				if(registroNuevo && esArchivoTemporal){
					registroNuevo = false;
					registroCorrecto = registroMasNuevo(registroCorrecto, registroSpliteado);
				}
			}while(!feof(archivo) && (esArchivoTemporal || strcmp(registroSpliteado[1], key)));
		}
		fclose(archivo);
		i++;
	}while(i < lengthListaDeBloques && (esArchivoTemporal || string_is_empty(registroCorrecto->value)));

	free(registro->value);
	return registroCorrecto;

}

void deRegistroSpliteadoANodoMemtable(char** registroSpliteado, nodo_memtable* registro){
	registro->timestamp = atoi(registroSpliteado[0]);
	registro->key = atoi(registroSpliteado[0]);
	registro->value = malloc(string_length(registroSpliteado[2]));
	strcpy(registro->value, registroSpliteado[2]);
	return;
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
	printf("HATSTAKJASDFJHASDF");
	while(temporalActual < cantidadDeTemporales){
		strcpy(nombreDelArchivo,  string_itoa(temporalActual));
		strcat(nombreDelArchivo, ".tmp");

		char* direccionDelArchivo = direccionDeArchivo(direccionDeLaTabla, nombreDelArchivo);
		char* registroActual = escanearArchivo(direccionDelArchivo, key, 1);

		registroCorrecto = registroMasNuevo(registroCorrecto, registroActual);
		temporalActual ++;
		//free(registroActual);
		//free(direccionDelArchivo);
	}

	//free(nombreDelArchivo);
	//free(direccionDelArchivo);

	return registroCorrecto;
	free(nombreDelArchivo);
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

nodo_memtable* registroMasNuevo(nodo_memtable* primerRegistro, nodo_memtable* segundoRegistro){
	if( string_is_empty(primerRegistro)){ //   strcmp(primerRegistro, "N")    		primerRegistro != NULL
		/*char** primerRegistroSpliteado = string_split(primerRegistro, ";");
		char** segundoRegistroSpliteado = string_split(segundoRegistro, ";");

		int timestampprimerRegistro = atoi(primerRegistroSpliteado[0]);
		int timestampSegundoRegistro = atoi(segundoRegistroSpliteado[0]);

		//liberarCharAsteriscoAsterisco(primerRegistroSpliteado);
		//liberarCharAsteriscoAsterisco(segundoRegistroSpliteado);
		*/
		if(primerRegistro->timestamp >= segundoRegistro->timestamp)
			//free(segundoRegistro->value);
			return primerRegistro;
	}
	//free(primerRegistro->value)
	return segundoRegistro;
}

uint8_t cantidadElementosCharAsteriscoAsterisco(char** array){
	uint8_t size;
	for(size = 0; array[size] != NULL; size++);
	return size;
}

void liberarCharAsteriscoAsterisco(char** array){
	string_iterate_lines(array, free);
	free(array);
//	uint8_t size = cantidadElementosCharAsteriscoAsterisco(array);
//	for(int i = 0; i < size; i++){
//		free(array[i]);
//	}
//	free(array);
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

	   if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))  // skip entries "." and ".."
		   continue;

	   full_path = calloc(path_len + strlen(entry->d_name) + 1, sizeof(char));
	   strcpy(full_path, direccion);
	   strcat(full_path, "/");
	   strcat(full_path, entry->d_name);

	   stat(full_path, &stat_entry);

	   if (S_ISDIR(stat_entry.st_mode) != 0) {  // recursively remove a nested directory
		   eliminarTabla(full_path);
		   continue;
	   }

	   if (unlink(full_path) == 0)
		   printf("Removed a file: %s\n", full_path);
	   else
		   printf("Can`t remove a file: %s\n", full_path);
   }

   if (rmdir(direccion) == 0)
	   printf("Removed a directory: %s\n", direccion);
   else
	   printf("Can`t remove a directory: %s\n", direccion);

   //free(direccion);
   closedir(dir);

   return;
}

datos_metadata* conseguirSuMetadataEn_datos_metadata(char* nombreDeTabla){
	char* direccionDeLaTabla = direccionDeTabla(nombreDeTabla);
	t_config* metadata = devolverMetadata(direccionDeLaTabla);
	datos_metadata* datos;

	datos->nombreTabla = nombreDeTabla;
	datos->consistencia = config_get_string_value(metadata, "CONSISTENCY");
	datos->particiones = config_get_int_value(metadata, "PARTITIONS");
	datos->tiempoDeCompactacion = config_get_int_value(metadata, "COMPACTION_TIME");

	return datos;
}

// --------------------------------------------------------------- //
// ----------------------- MANEJO DE BLOQUES --------------------- //
// --------------------------------------------------------------- //

t_config* obtenerMetadataDeFS(){
	t_config* metadata = config_create("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Metadata/Metadata.bin");
	return metadata;
}

void inicializarLog(){
	FILE* archivoDeLog = fopen("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/archivoDeLog", "r+");
    extern t_log *FSlog;
	FSlog = log_create(archivoDeLog, "filesystem.c", 0, LOG_LEVEL_DEBUG);
	return;
}

void logInfo(char* info){
	log_info(FSlog, info);
}

void logError(char* error){
	log_error(FSlog, error);
}

void inicializarBitmap(){
	logInfo("Filesystem: se procede a inicializar el bitmap");
	FILE* archivo;
	//extern t_bitarray *bitarray;
	t_bitarray *bitarrayAuxiliar;
	t_config* metadataLFS = obtenerMetadataDeFS();
	size_t cantidadDeBloques = config_get_int_value(metadataLFS, "BLOCKS");
	int cantidadDeChars = cantidadDeBloques/8;
	char* bitarrayDelArchivo = malloc(cantidadDeChars);

	if(archivo = fopen("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Metadata/Bitmap.bin", "r")){
		printf("El BITMAP ya estaba cargado.\n");
	}
	else{
		archivo = fopen("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Metadata/Bitmap.bin", "w");
		char* bitarrayNuevo = malloc((cantidadDeChars)+1);
		// lo usamos para crear y guardar un bitarray dentro del archivo, y posteriormente hacer el mmap del archivo.
		bitarrayAuxiliar = bitarray_create_with_mode(bitarrayNuevo, cantidadDeBloques, MSB_FIRST);
		// lo guardamos en el archivo
		fprintf(archivo, "%s", bitarrayAuxiliar->bitarray);
		bitarray_destroy(bitarrayAuxiliar);
		fclose(archivo);
	}



	if(archivo = open("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Metadata/Bitmap.bin", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)){

		bitarrayDelArchivo = mmap(NULL, cantidadDeChars, PROT_READ|PROT_WRITE, MAP_SHARED, archivo, 0);


	}
	else{

		archivo = open("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Metadata/Bitmap.bin", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		char* bitarrayNuevo = malloc((cantidadDeChars)+1);
		// lo usamos para crear y guardar un bitarray dentro del archivo, y posteriormente hacer el mmap del archivo.
		bitarrayAuxiliar = bitarray_create_with_mode(bitarrayNuevo, cantidadDeBloques, MSB_FIRST);
		// lo guardamos en el archivo
		fprintf(archivo, "%s", bitarrayAuxiliar->bitarray);
		bitarray_destroy(bitarrayAuxiliar);
		rewind(archivo);
		bitarrayDelArchivo = mmap(NULL, cantidadDeChars, PROT_READ|PROT_WRITE, MAP_SHARED, archivo, 0);

		printf("El BITMAP se cargo correctamente.\n");
	}
	logInfo("Filesystem: se inicializo el bitmap");
	bitarray = bitarray_create_with_mode(bitarrayDelArchivo, sizeof(bitarrayDelArchivo), MSB_FIRST);
	printf("holis");
	fclose(archivo);
	return;
}

void inicializarBloques(){
	logInfo("Filesystem: se procede a inicializar los bloques");
	printf("holis");
	t_config* metadataLFS = obtenerMetadataDeFS();
	size_t cantidadDeBloques = config_get_int_value(metadataLFS, "BLOCKS");
	char* direccion = direccionDeBloqueConInt(cantidadDeBloques - 1);
	FILE* bloque;
	if(bloque = fopen(direccion, "r")){
		fclose(bloque);
		printf("Los bloques estaban cargados.\n");

	}
	else{
		for(int i=0; i<cantidadDeBloques; i++){
			bloque = fopen(direccionDeBloqueConInt(i), "w");
			fclose(bloque);
		}
		printf("Los bloques < %i >fueron cargados correctamente.\n", cantidadDeBloques);
	}
	logInfo("Filesystem: se inicializaron los bloques");
	return;
}

char* direccionDeBloqueConInt(int numeroDeBloque){
	char* numeroDeBloqueEnChar = string_itoa(numeroDeBloque);
	char* nombreDeArchivo = malloc(string_length(numeroDeBloqueEnChar) + 4);
	strcpy(nombreDeArchivo, numeroDeBloqueEnChar);
	strcat(nombreDeArchivo, ".bin");
	int length = strlen("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Bloques") + strlen("/") + strlen(nombreDeArchivo) + 1;
	char* direccion = malloc(length);
	strcpy(direccion, "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Bloques");
	strcat(direccion, "/");
	strcat(direccion, nombreDeArchivo);
	return direccion;
}

void crearArchivoPuntoBin(char* direccionDeLaTabla, char* nombreDeArchivo){
	char* direccionDelArchivo = direccionDeArchivo(direccionDeLaTabla, nombreDeArchivo);
	FILE* archivo = fopen(direccionDelArchivo, "w");
	fprintf(archivo, "SIZE=0\nBLOCKS=[]\n");
	fclose(archivo);
	asignarBloque(direccionDelArchivo);
	return;
}

void asignarBloque(char* direccionDelArchivo){
	// buscar primer bloque libre en el bitarray
	// pasar numero a string
	char* bloqueLibre = string_itoa(primerBloqueLibre());
	// calcular longitud del string
	int longitudBloqueLibre = string_length(bloqueLibre);

	// vaciamos el contenido que podria tener el bloque previo a la asignacion
	char* direccionBloque = direccionDeBloque(bloqueLibre);
	FILE* nuevoBloque = fopen(direccionBloque, "w");
	fclose(nuevoBloque);
	// es necesario verificar si el archivo se creo correctamente?

	t_config* archivo = config_create(direccionDelArchivo);

	char* bloques = config_get_string_value(archivo, "BLOCKS");

	int length = string_length(bloques);

	char* nuevoValue = malloc(length) + longitudBloqueLibre + 4; // recordar acortar

	strcpy(nuevoValue, string_substring_until(bloques, length - 2));
	strcat(nuevoValue, ",");
	strcat(nuevoValue, bloqueLibre);
	strcat(nuevoValue, "]");
	strcat(nuevoValue, "\n");

	config_set_value(archivo, "BLOCKS", nuevoValue);
	config_save(archivo);
	config_destroy(archivo);

	return;
}

void liberarBloques(char* direccionDeArchivo){
	extern t_bitarray *bitarray;
	t_config* archivo = config_create(direccionDeArchivo);
	char** bloques = config_get_array_value(archivo, "BLOCKS");
	int length = cantidadElementosCharAsteriscoAsterisco(bloques);
	for(int i=0; i<length; i++ ){
		bitarray_clean_bit(bitarray, atoi(bloques[i]));
	}
	config_destroy(archivo);
	return;
}

int primerBloqueLibre(){
	int indice = 0;
	int ocupado = 0;

	pthread_mutex_lock(&mutexBitmap);

	extern t_bitarray *bitarray;
	ocupado = bitarray_test_bit(bitarray, indice);
	while(indice < bitarray->size && ocupado == 1){
		ocupado = bitarray_test_bit(bitarray, indice);
		indice++;
	}
	bitarray_set_bit(bitarray, indice);

	pthread_mutex_unlock(&mutexBitmap);

	return indice;
}

// ------------------------------------------------------------ //
// ----------------------- COMPACTACION ----------------------- //
// ------------------------------------------------------------ //


void compactacion(char* tabla){
	char* direccionTabla = direccionDeTabla(tabla);
	t_config* metadata = devolverMetadata(tabla);
	int tiempoDeCompactacion = config_get_int_value(metadata, "COMPACTION_TIME") / 8;
	t_list *listaDeClaves = list_create();

	while(1){
		sleep(tiempoDeCompactacion);
		logInfo("Filesystem: se procedera a hacer una compactacion");

		pasarLosTmpATmpc(direccionTabla);
		levantarClavesDeLosTmpc(direccionTabla, listaDeClaves);
		levantarClavesDeLasParticiones(direccionTabla, listaDeClaves);

		borrarLosTmpc(direccionTabla);
		borrarLosBin(direccionTabla);

		compactar(direccionTabla, listaDeClaves);

	}
	return;
}

void pasarLosTmpATmpc(char* direccionTabla){
	t_list *losTmp = listarArchivosDelTipo(direccionTabla, ".tmp");
	int length = list_size(losTmp);

	for(int i=0;i<length;i++){
		char* nombreArchivo = list_get(losTmp, i);
		int length1 = sizeof(direccionTabla) + 1 + sizeof(nombreArchivo);
		char* direccionVieja = malloc(length1);
		strcpy(direccionVieja, direccionTabla);
		strcat(direccionVieja, "/");
		strcat(direccionVieja, nombreArchivo);
		char* direccionNueva = malloc(length1 + 1);
		strcpy(direccionNueva, direccionVieja);
		strcat(direccionNueva, "c");

		rename(direccionVieja, direccionNueva);

		free(nombreArchivo);
		free(direccionVieja);
		free(direccionNueva);
	}
	return;
}


void levantarClavesDeLosTmpc(char* direccionTabla, t_list* listaDeClaves){
	t_list *losTmpc = listarArchivosDelTipo(direccionTabla, ".tmpc");
	t_config *archivo;
	int length = list_size(losTmpc);

	for(int i=0; i<length;i++){
		archivo = config_create(direccionDeArchivo(direccionTabla, list_get(losTmpc, i)));
		char** bloques = config_get_array_value(archivo, "BLOCKS");
		escanearBloques(bloques, listaDeClaves);
	}

	return;
}

void levantarClavesDeLasParticiones(char* direccionTabla, t_list* listaDeClaves){
	t_list *losBin = listarArchivosDelTipo(direccionTabla, ".bin");
	t_config * archivo;
	int length = list_size(losBin);

	for(int i=0; i<length;i++){
		archivo = config_create(direccionDeArchivo(direccionTabla, list_get(losBin, i)));
		char** bloques = config_get_array_value(archivo, "BLOCKS");
		escanearBloques(bloques, listaDeClaves);
	}
	list_destroy_and_destroy_elements(losBin, free);

	return;
}

t_list* listarArchivosDelTipo(char* direccionTabla, char* tipo){
	bool buscador(char* nombreArchivo){
		return string_ends_with(nombreArchivo, tipo);
	}

	t_list* archivosDeTabla = listarDirectorio(direccionTabla);

	return list_filter(archivosDeTabla, (void*)buscador);
}

void borrarLosTmpc(char* direccionTabla){
	t_list* losTmpc = listarArchivosDelTipo(direccionTabla, ".tmpc");
	//t_config * archivo;
	int length = list_size(losTmpc);
	for(int i=0; i<length;i++){
		char* nombreDeArchivo = list_get(losTmpc, i);
		liberarBloques(nombreDeArchivo);
		remove(direccionDeArchivo(direccionTabla, nombreDeArchivo));
		free(nombreDeArchivo);
	}
	return;
}

void borrarLosBin(char* direccionTabla){
	t_list* losBin = listarArchivosDelTipo(direccionTabla, ".bin");
	//t_config * archivo;
	int length = list_size(losBin);
	for(int i=0; i<length;i++){
		char* nombreDeArchivo = list_get(losBin, i);
		liberarBloques(nombreDeArchivo);
		remove(direccionDeArchivo(direccionTabla, nombreDeArchivo));
		free(nombreDeArchivo);
	}
	return;
}

void compactar(char* direccionTabla, t_list* listaDeClaves){
	t_config* metadata = devolverMetadata(direccionTabla);
	int cantidadDeParticiones = config_get_int_value(metadata, "PARTITIONS");
	int particion;
	nodo_memtable* registro;

	for(int i = 0; i < cantidadDeParticiones; i++){
		// char* direccionParticion = direccionDeParticion(direccionTabla, i);
		char* nombreDeParticion = malloc(10);
		strcpy(nombreDeParticion, string_itoa(i));
		strcat(nombreDeParticion, ".bin");
		crearArchivoPuntoBin(direccionTabla, nombreDeParticion);
		free(nombreDeParticion);
	}

	for(int j = 0; j < listaDeClaves->elements_count; j++){
		registro = list_get(listaDeClaves, j);
		particion = calcularParticion(registro->key, cantidadDeParticiones);
		escribirRegistroEnArchivo(direccionDeParticion(direccionTabla, particion), registro);
	}

}

char* direccionDeParticion(char* direccionTabla, int numeroDeParticion){
	char* nombreParticion = string_itoa(numeroDeParticion);
	int length = string_length(direccionTabla) + strlen(nombreParticion) + 5;
	char* direccionParticion = malloc(length);
	strcpy(direccionParticion, direccionTabla);
	strcat(direccionParticion, '/');
	strcat(direccionParticion, nombreParticion);
	strcat(direccionParticion, ".bin");
	return direccionParticion;
}

void escribirRegistroEnArchivo(char* direccionArchivo, nodo_memtable* registro){
	t_config* archivo = config_create(direccionArchivo);
	char** bloques = config_get_array_value(archivo, "BLOCKS");
	int size = config_get_int_value(archivo, "SIZE");
	int length = cantidadElementosCharAsteriscoAsterisco(bloques);
	char* direccionBloque = direccionDeBloque(bloques[length - 1]);
	FILE* bloque = fopen(direccionBloque, "a");
	char* registroString = pasarRegistroAString(registro);
	int longitudRegistro = string_length(registroString) + 1;
	int sobrante;
	int indice = 0;
	char* registroAuxiliar;



	while( longitudRegistro > 0){
		sobrante = (1 - size%tamanioMaximoDeArchivo) * tamanioMaximoDeArchivo;


		if( sobrante - longitudRegistro >= 0 ){
			fprintf(bloque, "%s\n", registro);
			longitudRegistro = 0;
			fclose(bloque);
			free(registroString);
			size += longitudRegistro;
		}
		else{
			fprintf(bloque, "s%", string_substring(registro, indice, sobrante));
			indice += sobrante;
			fclose(bloque);
			asignarBloque(direccionArchivo);
			liberarCharAsteriscoAsterisco(bloques);
			char** bloques = config_get_array_value(archivo, "BLOCKS");
			int length = cantidadElementosCharAsteriscoAsterisco(bloques);
			char* direccionBloque = direccionDeBloque(bloques[length - 1]);
			FILE* bloque = fopen(direccionBloque, "a");

			registroAuxiliar = malloc(longitudRegistro-sobrante);
			registroAuxiliar = string_substring_from(registroString, indice);
			free(registroString);

			registroString = malloc(longitudRegistro - sobrante);
			strcpy(registroString, registroAuxiliar);
			free(registroAuxiliar);
			size += sobrante;
			longitudRegistro -= sobrante;
		}
	}

	config_set_value(archivo, "SIZE", size);
	config_save(archivo);
	config_destroy(archivo);

}

/*
bool esTemporal(char* nombreArchivo){
	return esDelTipo(nombreArchivo, ".tmp");
}

bool esTemporalDeCompactacion(char* nombreArchivo){
	return esDelTipo(nombreArchivo, ".tmpc");
}

bool esUnaParticion(char* nombreArchivo){
	return esDelTipo(nombreArchivo, ".bin");
}

bool esDelTipo(char* nombreArchivo, char* tipo){
	return string_ends_with(nombreArchivo, tipo);
}
*/

char* direccionDeBloque(char* numeroDeBloque){
	int length = strlen(DIRECCION_BLOQUES) + strlen(numeroDeBloque) + 5;
	char* direccion = malloc(length);
	strcpy(direccion, DIRECCION_BLOQUES);
	strcat(direccion, numeroDeBloque);
	strcat(direccion, ".bin");
	return direccion;
}

void escanearBloques(char** listaDeBloques, t_list* listaDeRegistros){
	int lengthListaDeBloques = cantidadElementosCharAsteriscoAsterisco(listaDeBloques);
	char* registroIncompleto = malloc(tamanioMaximoDeRegistro);
	char* registro = malloc(tamanioMaximoDeRegistro);
	bool completo = true;

	for(int i=0;i<lengthListaDeBloques;i++){
		char* direccionDelBloque = direccionDeBloque(listaDeBloques[i]);
		FILE* archivo = fopen(direccionDelBloque, "r");

		if(archivo){
			while(!feof(archivo)){
				fgets(registro, tamanioMaximoDeRegistro, archivo);
				if(!completo){
					strcat(registroIncompleto, registro);
					strcpy(registro, registroIncompleto);
				}
				if(registroCompleto(registro)){

					insertarRegistroEnLaLista(listaDeRegistros, registro);
					completo = true;
				}
				else{
					strcpy(registroIncompleto, registro);
					completo = false;
				}
			}
			fclose(archivo);
		}
		else{
			error_show("No se pudo abrir el archivo");
		}

		free(direccionDelBloque);
	}

	free(registro);

	return;
}

void insertarRegistroEnLaLista(t_list* listaDeRegistros, char* registro){
	char** registroSpliteado = string_split(registro, ";");
	int lengthRegistros = list_size(listaDeRegistros);
	nodo_memtable* registroEnTabla;
	bool buscador(nodo_memtable elementoDeLista){
		return !strcmp(elementoDeLista.key, registroSpliteado[1]);
	}
	nodo_memtable* registroFormateado;
	registroFormateado->timestamp = atoi(registroSpliteado[0]);
	registroFormateado->key = atoi(registroSpliteado[1]);
	registroFormateado->value = malloc(tamanioMaximoDeRegistro);
	strcpy(registroFormateado, registroSpliteado[2]);

	if(registroEnTabla = list_find(listaDeRegistros, (void*) buscador)){

		printf("El registro viejo tiene los valores: \n timestamp = %i\n key = %i \n value = %s", registroEnTabla->timestamp, registroEnTabla->key, registroEnTabla->value);


		if(registroEnTabla->timestamp < registroFormateado->timestamp){

			registroEnTabla->timestamp = registroFormateado->timestamp;
			registroEnTabla->key = registroFormateado->key;
			// ~~~~~~~~~~~~ posiblemente tenga que hacer un free y un malloc de value ~~~~~~~~~~~~~//
			strcpy(registroEnTabla->value, registroFormateado->value);

			//nodo_memtable* registroDePrueba = list_find(listaDeRegistros, (void*)buscador);

			//printf("se ha cambiado el registro por uno mas nuevo\n El registro nuevo tiene los valores: \n timestamp = %i\n key = %i \n value = %s", registroDePrueba->timestamp, registroDePrueba->key, registroDePrueba->value);


		}
	}
	else{
		list_add(listaDeRegistros, registroFormateado);
	}
	return;
}

void setearTamanioMaximoRegistro(){
	t_config *config = obtenerConfigDeFS();
	int tamanioMaximo = config_get_int_value(config, "TAMAÑO_VALUE") + sizeof(uint32_t) + sizeof(uint16_t);
	extern uint16_t tamanioMaximoDeRegistro;
	tamanioMaximoDeRegistro = tamanioMaximo;
	return;
}

void setearTamanioMaximoArchivo(){
	t_config *config = obtenerMetadataDeFS();
	int tamanioMaximo = config_get_int_value(config, "BLOCK_SIZE");
	extern uint16_t tamanioMaximoDeArchivo;
	tamanioMaximoDeArchivo = tamanioMaximo;
	return;
}


bool registroCompleto(char* registro){
	return string_ends_with(registro, '\n');
}


