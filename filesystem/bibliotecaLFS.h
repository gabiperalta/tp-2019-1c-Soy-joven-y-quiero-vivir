
// AQUI SE ENCONTRARAN TODAS LAS FUNCIONES QUE USAREMOS INTERNAMENTE PARA LAS FUNCIONES QUE PIDE EL ENUNCIADO

#ifndef BIBLIOTECALFS_H_
#define BIBLIOTECALFS_H_


#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/error.h>
#include <commons/log.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>

t_dictionary *diccionario;
#define DIRECCION_TABLAS "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/tables/"

typedef struct {
	int timestamp;
	int key;
	char* value;
} nodo_memtable;

typedef struct {
	char* nombreTabla;
	char* consistencia;
	uint8_t particiones;
	uint16_t tiempoDeCompactacion;
} datos_metadata;


//const char** directorioDeTablas = "~/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/tables";
t_config* obtenerConfigDeFS();
void inicializarMemtable();
uint32_t getCurrentTime();
void crearTabla(char* nombreDeTabla, char* tipoDeConsistencia, int numeroDeParticiones, int tiempoDeCompactacion);
t_config* devolverMetadata(char* nombreDeTabla);
void imprimirMetadata(datos_metadata* datosDeMetadata);
char* direccionDeTabla(char* nombreDeTabla);
char* direccionDeArchivo(char* direccionDeLaTabla, char* nombreDeArchivo);
int calcularParticion(int key, int numeroDeParticiones);
int recorrerDirectorio(char* direccionDirectorio);
t_list* listarDirectorio(char* direccionDirectorio);
int crearArchivo(char* nombreDeTabla, char* nombreDeArchivo);
char* escanearArchivo(char* direccionDelArchivo, char* key, int esArchivoTemporal);
char* buscarEnTemporales(char* direccionDeLaTabla,char* key);
char* buscarMemoriaTemporal(char* nombreDeTabla, char* key);
char* pasarRegistroAString(nodo_memtable* registro);
char* registroMasNuevo(char* primerRegistro, char* segundoRegistro);
uint8_t cantidadElementosCharAsteriscoAsterisco(char** array);
void liberarCharAsteriscoAsterisco(char** array);
int elArchivoEsDelTipo(char* archivo, char* tipoQueDebeSer);
void dump();
void pasarAArchivoTemporal(char* nombreDeTabla, t_list* registros);
void eliminarTabla(char* nombreDeTabla);
int existeLaTabla(char* nombreDeTabla);
datos_metadata* conseguirSuMetadataEn_datos_metadata(char* nombreDeTabla);
t_config* obtenerMetadataDeFS();
void inicializarBitmap();
void crearArchivoPuntoBin(char* direccionDeLaTabla, char* nombreDeArchivo);
void asignarBloque(char* direccionDelArchivo);
int primerBloqueLibre();


#endif /* BIBLIOTECALFS_H_ */
