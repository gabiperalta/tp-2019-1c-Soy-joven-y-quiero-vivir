
// AQUI SE ENCONTRARAN TODAS LAS FUNCIONES QUE USAREMOS INTERNAMENTE PARA LAS FUNCIONES QUE PIDE EL ENUNCIADO

#ifndef BIBLIOTECALFS_H_
#define BIBLIOTECALFS_H_


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




//const char** directorioDeTablas = "~/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/tables";

void crearTabla(char* nombreDeTabla, char* tipoDeConsistencia, int numeroDeParticiones, int tiempoDeCompactacion);
t_config* devolverMetadata(char* nombreDeTabla);
char* direccionDeTabla(char* nombreDeTabla);
char* direccionDeArchivo(char* direccionDeLaTabla, char* nombreDeArchivo);
int calcularParticion(int key, int numeroDeParticiones);
int recorrerDirectorio(char* direccionDirectorio);
int crearArchivo(char* nombreDeTabla, char* nombreDeArchivo);
char* escanearArchivo(char* direccionDelArchivo, char* key, int esArchivoTemporal);
char* buscarEnParticiones(char* direccionDeLaTabla,char* key);
char* registroMasNuevo(char* primerRegistro, char* segundoRegistro);
int elArchivoEsDelTipo(char* archivo, char* tipoQueDebeSer);
// bool existeLaTabla(char* nombreDeTabla);

#endif /* BIBLIOTECALFS_H_ */
