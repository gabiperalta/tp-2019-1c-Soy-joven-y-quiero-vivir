#ifndef BIBLIOTECA_SOCKETS_H_
#define BIBLIOTECA_SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PUERTO_ESCUCHA_FS 8090
#define PUERTO_ESCUCHA_MEM 8095



int escuchar(int puerto);
int conectarseA(char *ip, int puerto);
int aceptarConexion(int socketEscucha);


#endif
