#include "bibliotecaLFS.h"

// PROTOTIPOS


// PERMITE LA OBTENCION DEL VALOR DE UNA KEY DENTRO DE UNA TABLA

void inicializarMemtable(){
	extern t_dictionary *diccionario;
	diccionario = dictionary_create();
	return;
}

void fijarPuntoDeMontaje(){
	extern char* punto_de_montaje;
	t_config* config = obtenerConfigDeFS();
	char* puntoDeMontajeDelConfig = config_get_string_value(config, "PUNTO_MONTAJE");
	punto_de_montaje = malloc(strlen(puntoDeMontajeDelConfig) + 1);
	strcpy(punto_de_montaje, puntoDeMontajeDelConfig);
	config_destroy(config);
	return;
}

void inicializarListaDeTablas(){
	extern t_list *listaDeTablas;
	listaDeTablas = list_create();

	char* direccion_tablas = obtenerDireccionDirectorio("tables");


	t_list* nombresDeTablas = listarDirectorio(direccion_tablas);


	for(int i = 0; i<nombresDeTablas->elements_count; i++){
		ingresarTablaEnListaDeTablas(list_get(nombresDeTablas, i));
	}
}

void ingresarTablaEnListaDeTablas(char* nombreDeTabla){
	datos_metadata* datosMetadata = malloc(sizeof(datos_metadata));
	nodo_lista_tablas* nodoListaTablas = malloc(sizeof(nodo_lista_tablas));
	datosMetadata = conseguirSuMetadataEn_datos_metadata(nombreDeTabla);
	nodoListaTablas->nombreTabla = malloc(strlen(nombreDeTabla) + 1);
	strcpy(nodoListaTablas->nombreTabla, nombreDeTabla);
	nodoListaTablas->consistencia = malloc(strlen(datosMetadata->consistencia) + 1);
	strcpy(nodoListaTablas->consistencia, datosMetadata->consistencia);
	nodoListaTablas->particiones = datosMetadata->particiones;
	nodoListaTablas->tiempoDeCompactacion = datosMetadata->tiempoDeCompactacion;
	pthread_mutex_t unMutex;
	pthread_mutex_init(&unMutex, NULL);
	list_add(listaDeTablas, nodoListaTablas);
}

void sacarDeLaListaDeTablas(char* nombreDeTabla){
	bool buscador(nodo_lista_tablas* elemento){
		return !strcmp(elemento->nombreTabla, nombreDeTabla);
	}
	return list_remove_and_destroy_by_condition(listaDeTablas, (void*)buscador, (void*)destructorDeTablaDeLista);
}

void destructorDeTablaDeLista(nodo_lista_tablas* tabla){
	free(tabla->consistencia);
	free(tabla->nombreTabla);
	pthread_mutex_destroy(&tabla->mutex);
	return;
}

void inicializarHilosDeCompactacion(){

 	char* direccion_tablas = obtenerDireccionDirectorio("tables/");
 	char* nombreDeTabla = malloc(15);

	t_list* nombresDeTablas = listarDirectorio(direccion_tablas);

	for(int i = 0; i<nombresDeTablas->elements_count; i++){
		strcpy(nombreDeTabla, list_get(nombresDeTablas, i));
		iniciarSuHiloDeCompactacion(nombreDeTabla);
	}
}

void iniciarSuHiloDeCompactacion(char* nombreDeTabla){
	pthread_t hiloDeCompactacion;
	/*void compactationThread(){
		compactacion(nombreDeTabla);
	}*/

	pthread_create(&hiloDeCompactacion, NULL, (void*)compactacion, (void*)nombreDeTabla);
	printf("hasta aca NO funciona\n");
	pthread_detach(hiloDeCompactacion);
}

t_config* obtenerConfigDeFS(){
	t_config* config = config_create("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Config.bin"); // ../filesystem/Config.bin
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

bool existeLaTabla(char* nombreDeTabla){

	bool buscador(nodo_lista_tablas* elemento){

		return !strcmp(elemento->nombreTabla, nombreDeTabla);
	}

	return list_any_satisfy(listaDeTablas, (void*)buscador);
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
			contador ++;
			/*if(!crearArchivoPuntoBin(direccion, nombreDeParticion)){
				printf("Se creo la particion < %i >\n", contador);
			}else{
				printf("No se creo la particion < %i >, se volvera a intentar.\n", contador);
			}*/
		}
	}
	else{
		printf("No se pudo crear la tabla.\n");
	}
	// free(direccion);

	return;
}

t_config* devolverMetadata(char* direccionDeLaTabla){
	char* direccion =direccionDeArchivo(direccionDeLaTabla, "Metadata");
	t_config* metadata;
	metadata = config_create(direccion);
	// free(direccion);
	return metadata;
}

void imprimirMetadata(datos_metadata* datosDeMetadata){
	printf("\nMetadata de < %s >:\n\n", datosDeMetadata->nombreTabla);
	printf("CONSISTENCY = %s \n "
			"PARTITIONS = %i \n"
			"COMPACTION_TIME = %i \n\n", datosDeMetadata->consistencia, datosDeMetadata->particiones, datosDeMetadata->tiempoDeCompactacion);
	return;
}

char* obtenerDireccionDirectorio(char* nombreDirectorio){
	int length = strlen(punto_de_montaje) + strlen(nombreDirectorio);
	char* direccion = malloc(length + 1);
	strcpy(direccion, punto_de_montaje);
	strcat(direccion, nombreDirectorio);
	return direccion;
}



char* direccionDeTabla(char* nombreDeTabla){
	char* direccion_tablas = obtenerDireccionDirectorio("tables/");
	int length = strlen(direccion_tablas) + strlen(nombreDeTabla) + 1;
	char* direccion = malloc(length);
	strcpy(direccion, direccion_tablas);
	strcat(direccion, nombreDeTabla);
	printf("se quiere hacer un puto free.\n");
	free(direccion_tablas);
	printf("se hizo un puto free.\n");
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
/*int recorrerDirectorio(char* direccionDirectorio) {
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
}*/

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
					agregarDirectorioALaLista(listaDeArchivos, stream->d_name);
					//list_add(listaDeArchivos, stream->d_name);
				//array[indice] = stream->d_name;
				//indice++;
						printf("\n El nombre es:%s\n", stream->d_name);
				}
			}

			//printf("\n\nSe encotro un total de %i archivos\n", contador);
			printf("\n a continuacion se listaran los archivos identificados:\n");
			for(int i = 0; i < listaDeArchivos->elements_count; i++){
				printf("%s\n", list_get(listaDeArchivos , i));
			}
			char* nombreArchivo;
			/*for(int i=0; i<listaDeArchivos->elements_count; i++){
				nombreArchivo = list_get(listaDeArchivos, i);
				printf("%s\n", nombreArchivo);
			}*/


			if (closedir(directorio) == -1)
			{
					error_show("No se pudo cerrar el directorio\n");
					return NULL;
			}
			printf("\nSe cerro correctamente el directorio\n");

	return listaDeArchivos;
}

void agregarDirectorioALaLista(t_list* listaDeArchivos, char* unNombreArchivo){
	char* nombreDeArchivo = malloc(strlen(unNombreArchivo));
	strcpy(nombreDeArchivo, unNombreArchivo);
	list_add(listaDeArchivos, nombreDeArchivo);
	return;
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
	printf("Se hara un DUMP\n");
	logInfo("Se hizo un DUMP.");

	if(!dictionary_is_empty(diccionario)){
	dictionary_iterator(diccionario, (void*)pasarAArchivoTemporal);
	dictionary_clean_and_destroy_elements(diccionario, (void*)list_destroy);

	}
	else{
		//printf("Se intento hacer un DUMP pero la memoria temporal esta vacia.\n");
		logInfo("Se intento hacer un DUMP pero la memoria temporal esta vacia.");
	}
	// listmap()
	}
	return;
}

void pasarAArchivoTemporal(char* nombreDeTabla, t_list* registros){
	char* direccion = direccionDeTabla(nombreDeTabla);
	t_list* archivosTemporales = listarArchivosDelTipo(direccion, ".tmp");
	uint8_t numeroDeTemporal = list_size(archivosTemporales);
	list_destroy_and_destroy_elements(archivosTemporales, free);
	// uint8_t numeroDeTemporal = recorrerDirectorio(direccion);
	char* numeroArchivo = string_itoa(numeroDeTemporal);
	int length = strlen(numeroArchivo) + strlen(".tmp") + 1;
	char* nombreDeArchivo = malloc(length);
	strcpy(nombreDeArchivo, numeroArchivo);
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
	nodo_memtable* registroCorrecto = NULL;
	nodo_memtable* registro;
	//registroCorrecto->value = malloc(tamanioMaximoDeRegistro - sizeof(uint16_t) - sizeof(uint32_t));
	//registro->value = malloc(tamanioMaximoDeRegistro - sizeof(uint16_t) - sizeof(uint32_t));
	char** registroSpliteado;
	//strcpy(registroCorrecto->value, "");  // PUEDE GENERAR ERROR POR NO ASIGNARLE MEMORIA
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
					registroSpliteado = string_split(unRegistro, ";");
					if(!strcmp(registroSpliteado[1], key)){
						if( registroCorrecto == NULL){
							registroNuevo = true;
							deRegistroSpliteadoANodoMemtable(registroSpliteado, registroCorrecto);
						}else{
							deRegistroSpliteadoANodoMemtable(registroSpliteado, registro);
						}
					}
					completo = true;
				}
				else{
					strcpy(registroIncompleto, unRegistro);
					completo = false;
				}
				if(registroNuevo && esArchivoTemporal){
					registroNuevo = false;
					registroCorrecto = registroMasNuevo(registroCorrecto, registro);
				}
			}while(!feof(archivo) && (esArchivoTemporal || strcmp(registroSpliteado[1], key)));
		}
		fclose(archivo);
		i++;
		free(direccionDelBloque);
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


nodo_memtable* buscarEnTemporales(char* direccionDeLaTabla,char* key){
	//char* registroActual = malloc(100);
	t_list* archivosTemporales = listarArchivosDelTipo(direccionDeLaTabla, ".tmp");
	uint8_t cantidadDeTemporales = list_size(archivosTemporales);
	list_destroy_and_destroy_elements(archivosTemporales, free);
	//uint8_t cantidadDeTemporales = recorrerDirectorio(direccionDeLaTabla);
	char* nombreDelArchivo = malloc(20);
	uint8_t temporalActual = 0;
	nodo_memtable* registroCorrecto = NULL;
	nodo_memtable* registroActual;


	//char* direccionDelArchivo = malloc(sizeof(direccionDeLaTabla) + 7); // 7 seria el tamanio estimado del nombre de un n.tmp
	//registroCorrecto = NULL;
	while(temporalActual < cantidadDeTemporales){
		strcpy(nombreDelArchivo,  string_itoa(temporalActual));
		strcat(nombreDelArchivo, ".tmp");

		char* direccionDelArchivo = direccionDeArchivo(direccionDeLaTabla, nombreDelArchivo);
		registroActual = escanearArchivo(direccionDelArchivo, key, 1);

		registroCorrecto = registroMasNuevo(registroCorrecto, registroActual);
		temporalActual ++;
		//free(registroActual);
		//free(direccionDelArchivo);
	}

	//free(nombreDelArchivo);
	//free(direccionDelArchivo);
	//free(nombreDelArchivo);

	return registroCorrecto;
}

nodo_memtable* buscarMemoriaTemporal(char* nombreDeTabla, char* key){
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
		return NULL;

	//char* registroFinal = pasarRegistroAString( registroCorrecto);

	return registroCorrecto;
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
	if( primerRegistro == NULL){ //   strcmp(primerRegistro, "N")    		primerRegistro != NULL

		if(primerRegistro->timestamp >= segundoRegistro->timestamp)
			return primerRegistro;
			//free(segundoRegistro->value);
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

   sacarDeLaListaDeTablas(nombreDeTabla);

   return;
}

datos_metadata* conseguirSuMetadataEn_datos_metadata(char* nombreDeTabla){
	char* direccionDeLaTabla = direccionDeTabla(nombreDeTabla);
	t_config* metadata = devolverMetadata(direccionDeLaTabla);
	datos_metadata* datos = malloc(sizeof(datos_metadata));


	datos->nombreTabla = malloc(strlen(nombreDeTabla) + 1);
	strcpy(datos->nombreTabla, nombreDeTabla);
	datos->consistencia = malloc(4);
	strcpy(datos->consistencia, config_get_string_value(metadata, "CONSISTENCY"));
	datos->particiones = config_get_int_value(metadata, "PARTITIONS");
	datos->tiempoDeCompactacion = config_get_int_value(metadata, "COMPACTION_TIME");

	config_destroy(metadata);

	return datos;
}

// --------------------------------------------------------------- //
// ----------------------- MANEJO DE BLOQUES --------------------- //
// --------------------------------------------------------------- //

t_config* obtenerMetadataDeFS(){
	char* direccionMetadata = malloc(strlen(punto_de_montaje) + strlen("Metadata/Metadata.bin"));
	direccionMetadata = obtenerDireccionDirectorio("Metadata/Metadata.bin");
	t_config* metadata = config_create(direccionMetadata);
	// free(direccionMetadata);
	return metadata;
}

void inicializarLog(){
	char* direccionArchivoLog = malloc(strlen(punto_de_montaje) + strlen("archivoDeLog") + 1);
	direccionArchivoLog = obtenerDireccionDirectorio("archivoDeLog");
	FILE* archivoDeLog;

	archivoDeLog = fopen(direccionArchivoLog, "a");

	if(!archivoDeLog){
		archivoDeLog = fopen(direccionArchivoLog, "w");
	}
	fclose(archivoDeLog);
    extern t_log *FSlog;
	FSlog = log_create(direccionArchivoLog, "filesystem.c", 0, LOG_LEVEL_DEBUG);
	//free(direccionArchivoLog);
	logInfo("~~~~~~~~~~ FILESYSTEM ~~~~~~~~~~");
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
	extern t_bitarray *bitarray;
	t_bitarray *bitarrayAuxiliar;
	t_config* metadataLFS = obtenerMetadataDeFS();
	size_t cantidadDeBloques = config_get_int_value(metadataLFS, "BLOCKS");
	int cantidadDeChars = cantidadDeBloques/8;
	char* bitarrayDelArchivo = malloc(cantidadDeChars);

	char* direccion_bitmap = obtenerDireccionDirectorio("Metadata/Bitmap.bin");

	if(archivo = fopen(direccion_bitmap, "r")){
		printf("El BITMAP ya estaba cargado.\n");
	}
	else{
		archivo = fopen(direccion_bitmap, "w"); // lo usamos para crear y guardar un bitarray dentro del archivo, y posteriormente hacer el mmap del archivo.
		char* bitarrayNuevo = malloc((cantidadDeChars)+1);
		bitarrayAuxiliar = bitarray_create_with_mode(bitarrayNuevo, cantidadDeBloques, MSB_FIRST);
		fprintf(archivo, "%s", bitarrayAuxiliar->bitarray);
		bitarray_destroy(bitarrayAuxiliar);
		fclose(archivo);
	}



	if(archivo = open(direccion_bitmap, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)){

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
	close(archivo);
	config_destroy(metadataLFS);
	return;
}

void inicializarBloques(){
	logInfo("Filesystem: se procede a inicializar los bloques");
	t_config* metadataLFS = obtenerMetadataDeFS();
	size_t cantidadDeBloques = config_get_int_value(metadataLFS, "BLOCKS");
	char* direccion = direccionDeBloqueConInt(cantidadDeBloques - 1);
	FILE* bloque;
	char* direccionDeUnBloque = malloc(20);

	if(bloque = fopen(direccion, "r")){
		fclose(bloque);
		printf("Los bloques estaban cargados.\n");

	}
	else{
		for(int i=0; i<cantidadDeBloques; i++){
			direccionDeUnBloque = direccionDeBloqueConInt(i);
			bloque = fopen(direccionDeUnBloque, "w");
			fclose(bloque);
		}
		printf("Los bloques < %i >fueron cargados correctamente.\n", cantidadDeBloques);
	}
	logInfo("Filesystem: se inicializaron los bloques");
	config_destroy(metadataLFS);
	return;
}

char* direccionDeBloqueConInt(int numeroDeBloque){
	int length2 = strlen(string_itoa(numeroDeBloque));
	char* numeroDeBloqueEnChar = malloc(length2);
	strcpy(numeroDeBloqueEnChar, string_itoa(numeroDeBloque));
	char* nombreDeArchivo = malloc(string_length(numeroDeBloqueEnChar) + 4);
	strcpy(nombreDeArchivo, numeroDeBloqueEnChar);
	strcat(nombreDeArchivo, ".bin");
	char* direccionDeBloques = obtenerDireccionDirectorio("Bloques");
	int length = strlen(direccionDeBloques) + strlen("/") + strlen(nombreDeArchivo) + 1;
	char* direccion = malloc(length);
	strcpy(direccion, direccionDeBloques);
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

	char* nuevoValue = malloc(length + longitudBloqueLibre + 4); // recordar acortar

	if(length < 3){
		strcpy(nuevoValue, string_substring_until(bloques, length - 1));
		strcat(nuevoValue, bloqueLibre);
		strcat(nuevoValue, "]");
		strcat(nuevoValue, "\n");
	}else{
		strcpy(nuevoValue, string_substring_until(bloques, length - 1));
		strcat(nuevoValue, ",");
		strcat(nuevoValue, bloqueLibre);
		strcat(nuevoValue, "]");
		strcat(nuevoValue, "\n");
	}

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


void compactacion(char* nombreTabla){
	char* direccionTabla = direccionDeTabla(nombreTabla);
	int tiempoDeCompactacion = obtenerTiempoDeCompactacion(nombreTabla);
	t_list *listaDeClaves = list_create();
	pthread_mutex_t mutex = obtenerMutex(nombreTabla);

	bool buscador(nodo_lista_tablas* elemento){
		return !strcmp( elemento->nombreTabla, nombreTabla);
	}
	nodo_lista_tablas* tabla = list_find( listaDeTablas, (void*)buscador);

	while(1){
		sleep(tiempoDeCompactacion);

		printf("se hara una compactacion\n");

		if(tabla == NULL){
			break;
		}

		pthread_mutex_lock(&mutex);

		logInfo("Filesystem: se procedera a hacer una compactacion");

		pasarLosTmpATmpc(direccionTabla);
		levantarClavesDeLosTmpc(direccionTabla, listaDeClaves);
		levantarClavesDeLasParticiones(direccionTabla, listaDeClaves);

		borrarLosTmpc(direccionTabla);
		borrarLosBin(direccionTabla);

		compactar(direccionTabla, listaDeClaves);

		pthread_mutex_unlock(&mutex);


	}
	return;
}

pthread_mutex_t obtenerMutex(char* nombreDeTabla){

	bool buscador(nodo_lista_tablas* elemento){
		return !strcmp(elemento->nombreTabla, nombreDeTabla);
	}

	nodo_lista_tablas* tabla = list_take(listaDeTablas, (void*)buscador);
	return tabla->mutex;
}

uint32_t obtenerTiempoDeCompactacion(char* nombreDeTabla){
	bool buscador(nodo_lista_tablas* elemento){
		return !strcmp(elemento->nombreTabla, nombreDeTabla);
	}

	nodo_lista_tablas* tabla = list_take(listaDeTablas, (void*)buscador);
	return tabla->tiempoDeCompactacion;
}

uint8_t obtenerParticiones(char* nombreDeTabla){
	bool buscador(nodo_lista_tablas* elemento){
		return !strcmp(elemento->nombreTabla, nombreDeTabla);
	}

	nodo_lista_tablas* tabla = list_take(listaDeTablas, (void*)buscador);
	return tabla->particiones;
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

	char* direccion_Bloques = obtenerDireccionDirectorio("Bloques/");

	int length = strlen(direccion_Bloques) + strlen(numeroDeBloque) + 5;
	char* direccion = malloc(length);
	strcpy(direccion, direccion_Bloques);
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
	config_destroy(config);
	return;
}


bool registroCompleto(char* registro){
	return string_ends_with(registro, '\n');
}


