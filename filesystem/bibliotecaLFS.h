
// AQUI SE ENCONTRARAN TODAS LAS FUNCIONES QUE USAREMOS INTERNAMENTE PARA LAS FUNCIONES QUE PIDE EL ENUNCIADO

#ifndef BIBLIOTECALFS_H_
#define BIBLIOTECALFS_H_

#include <commons/string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>


const char* directorioDeTablas = "~/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/tables";

void crearTabla(char* nombreDeTabla, char* tipoDeConsistencia, int numeroDeParticiones, int tiempoDeCompactacion);
// bool existeLaTabla(char* nombreDeTabla);

#endif /* BIBLIOTECALFS_H_ */
