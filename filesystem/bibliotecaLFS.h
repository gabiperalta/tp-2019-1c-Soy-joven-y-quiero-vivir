
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
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fcntl.h>

t_dictionary *diccionario;
t_bitarray *bitarray;
pthread_mutex_t mutexBitmap;
t_log *FSlog;
uint16_t tamanioMaximoDeRegistro;
uint16_t tamanioMaximoDeArchivo;


#define DIRECCION_TABLAS "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/tables/"
#define DIRECCION_BLOQUES "/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Bloques/"

typedef struct {
	uint32_t timestamp;
	uint16_t key;
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
nodo_memtable* escanearArchivo(char* direccionDelArchivo, char* key, int esArchivoTemporal);
nodo_memtable* buscarRegistroMasNuevo(char** listaDeBloques, char* key, int esArchivoTemporal);
void deRegistroSpliteadoANodoMemtable(char** registroSpliteado, nodo_memtable* registro);
char* buscarEnTemporales(char* direccionDeLaTabla,char* key);
char* buscarMemoriaTemporal(char* nombreDeTabla, char* key);
char* pasarRegistroAString(nodo_memtable* registro);
nodo_memtable* registroMasNuevo(nodo_memtable* primerRegistro, nodo_memtable* segundoRegistro);
uint8_t cantidadElementosCharAsteriscoAsterisco(char** array);
void liberarCharAsteriscoAsterisco(char** array);
int elArchivoEsDelTipo(char* archivo, char* tipoQueDebeSer);
void dump();
void pasarAArchivoTemporal(char* nombreDeTabla, t_list* registros);
void eliminarTabla(char* nombreDeTabla);
int existeLaTabla(char* nombreDeTabla);
datos_metadata* conseguirSuMetadataEn_datos_metadata(char* nombreDeTabla);
t_config* obtenerMetadataDeFS();
void inicializarLog();
void logInfo(char* info);
void logError(char* error);
void inicializarBitmap();
void inicializarBloques();
char* direccionDeBloqueConInt(int numeroDeBloque);
void crearArchivoPuntoBin(char* direccionDeLaTabla, char* nombreDeArchivo);
void asignarBloque(char* direccionDelArchivo);
void liberarBloques(char* direccionDeArchivo);
int primerBloqueLibre();

void compactacion(char* tabla);
void pasarLosTmpATmpc(char* direccionTabla);
void levantarClavesDeLosTmpc(char* direccionTabla, t_list* listaDeClaves);
void levantarClavesDeLasParticiones(char* direccionTabla, t_list* listaDeClaves);
void borrarLosTmpc(char* direccionTabla);
void borrarLosBin(char* direccionTabla);
void compactar(char* direccionTabla, t_list* listaDeClaves);
bool esTemporal(char* nombreArchivo);
bool esTemporalDeCompactacion(char* nombreArchivo);
bool esDelTipo(char* nombreArchivo, char* tipo);
char* direccionDeBloque(char* numeroDeBloque);
void escanearBloques(char** listaDeBloques, t_list* listaDeRegistros); // REEMPLAZO DE ESCANEAR ARCHIVO

t_list* listarArchivosDelTipo(char* direccionTabla, char* tipo);
char* direccionDeParticion(char* direccionTabla, int numeroDeParticion);
void escribirRegistroEnArchivo(char* direccionArchivo, nodo_memtable* registro);
void insertarRegistroEnLaLista(t_list* listaDeRegistros, char* registro);
void setearTamanioMaximoRegistro();
void setearTamanioMaximoArchivo();
bool registroCompleto(char* registro);

#endif /* BIBLIOTECALFS_H_ */
