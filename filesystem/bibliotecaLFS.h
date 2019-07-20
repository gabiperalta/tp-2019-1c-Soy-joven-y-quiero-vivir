
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
#include "../biblioteca/biblioteca_sockets.h"

t_dictionary *diccionario;
t_list *listaDeTablas;
t_bitarray *bitarray;
pthread_mutex_t mutexBitmap;
t_log *FSlog;
uint16_t tamanioMaximoDeRegistro;
uint16_t tamanioMaximoDeArchivo;
char* punto_de_montaje;


typedef struct {
	uint32_t timestamp;
	uint16_t key;
	char* value;
} nodo_memtable;

typedef struct {
	char* nombreTabla;
	char* consistencia;
	uint8_t particiones;
	uint32_t tiempoDeCompactacion;
} datos_metadata;

typedef struct {

	char* nombreTabla;
	char* consistencia;
	uint8_t particiones;
	uint32_t tiempoDeCompactacion;
	pthread_mutex_t mutex;
} nodo_lista_tablas;

//const char** directorioDeTablas = "~/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/tables";
t_config* obtenerConfigDeFS();
void inicializarMemtable();
void fijarPuntoDeMontaje();
void ingresarTablaEnListaDeTablas(char* nombreDeTabla);
void sacarDeLaListaDeTablas(char* nombreDeTabla);
void destructorDeTablaDeLista(nodo_lista_tablas* tabla);
void inicializarHilosDeCompactacion();
void inicializarListaDeTablas();
void iniciarSuHiloDeCompactacion(char* nombreDeTabla);
void inicializarHilosDeCompactacion();
uint32_t getCurrentTime();
void crearTabla(char* nombreDeTabla, char* tipoDeConsistencia, int numeroDeParticiones, int tiempoDeCompactacion);
t_config* devolverMetadata(char* nombreDeTabla);
void imprimirMetadata(datos_metadata* datosDeMetadata);
char* obtenerDireccionDirectorio(char* nombreDirectorio);
char* direccionDeTabla(char* nombreDeTabla);
char* direccionDeArchivo(char* direccionDeLaTabla, char* nombreDeArchivo);
int calcularParticion(int key, int numeroDeParticiones);
int recorrerDirectorio(char* direccionDirectorio);
t_list* listarDirectorio(char* direccionDirectorio);
void agregarDirectorioALaLista(t_list* listaDeArchivos, char* unNombreArchivo);
int crearArchivo(char* nombreDeTabla, char* nombreDeArchivo);
nodo_memtable* escanearArchivo(char* direccionDelArchivo, char* key, int esArchivoTemporal);
nodo_memtable* buscarRegistroMasNuevo(char** listaDeBloques, char* key, int esArchivoTemporal);
void deRegistroSpliteadoANodoMemtable(char** registroSpliteado, nodo_memtable* registro);
nodo_memtable* buscarEnTemporales(char* direccionDeLaTabla,char* key);
nodo_memtable* buscarMemoriaTemporal(char* nombreDeTabla, char* key);
char* pasarRegistroAString(nodo_memtable* registro);
nodo_memtable* registroMasNuevo(nodo_memtable* primerRegistro, nodo_memtable* segundoRegistro);
void liberarCharAsteriscoAsterisco(char** array);
int elArchivoEsDelTipo(char* archivo, char* tipoQueDebeSer);
void dump();
void destructorListaMemtable(t_list* listaMemtable);
void eliminarNodoMemtable(nodo_memtable* elemento);
void pasarAArchivoTemporal(char* nombreDeTabla, t_list* registros);
void eliminarTabla(char* nombreDeTabla);
bool existeLaTabla(char* nombreDeTabla);
datos_metadata* conseguirSuMetadataEn_datos_metadata(char* nombreDeTabla);
t_config* obtenerMetadataDeFS();
void inicializarLog();
void logInfo(char* info);
void logError(char* error);
void loguearMetadata(datos_metadata* metadata);
void inicializarBitmap();
void inicializarBloques();
char* direccionDeBloqueConInt(int numeroDeBloque);
void crearArchivoPuntoBin(char* direccionDeLaTabla, char* nombreDeArchivo);
void asignarBloque(char* direccionDelArchivo);
void liberarBloques(char* direccionDeArchivo);
int primerBloqueLibre();

void compactacion(char* tabla);
pthread_mutex_t obtenerMutex(char* nombreDeTabla);
uint32_t obtenerTiempoDeCompactacion(char* nombreDeTabla);
uint8_t obtenerParticiones(char* nombreDeTabla);
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
