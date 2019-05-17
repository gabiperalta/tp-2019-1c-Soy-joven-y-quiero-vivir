/*
 * funcionesLFS.h
 *
 *  Created on: 14 may. 2019
 *      Author: utnso
 */

// AQUI SE ENCONTRARAN TODAS LAS FUNCIONES QUE PIDE EL ENUNCIADO

#ifndef FUNCIONESLFS_H_
#define FUNCIONESLFS_H_

#include <commons/string.h>
#include <commons/config.h>
#include <commons/error.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include "bibliotecaLFS.h"


void selectLFS(char* nombreDeTabla, char* key);
void insertLFS(char* nombreDeTabla, char* key, char* valor, char* timestamp);
void createLFS(char* nombreDeTabla, char* tipoDeConsistencia, char* numeroDeParticiones, char* tiempoDeCompactacion);
void describeLSF(char* nombreDeTabla);
void dropLSF(char* nombreDeTabla);


#endif /* FUNCIONESLFS_H_ */
