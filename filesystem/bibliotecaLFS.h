#ifndef BIBLIOTECALFS_H_
#define BIBLIOTECALFS_H_

#include <commons/string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void selectLFS(char* nombreDeTabla, char* key);
void insertLFS(char* nombreDeTabla, char* key, char* valor, char* timestamp);
void createLFS(char* nombreDeTabla, char* tipoDeConsistencia, char* numeroDeParticiones, char* tiempoDeCompactacion);
void describeLSF(char* nombreDeTabla);
void dropLSF(char* nombreDeTabla);

#endif /* BIBLIOTECALFS_H_ */
