#ifndef BIBLIOTECA_SOCKETS_H_
#define BIBLIOTECA_SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>



int escuchar(int puerto);
int conectarseA(char *ip, int puerto);
int aceptarConexion(int socketEscucha);


#endif
