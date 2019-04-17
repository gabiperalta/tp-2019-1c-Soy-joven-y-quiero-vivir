#ifndef BIBLIOTECA_SOCKETS_H_
#define BIBLIOTECA_SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PUERTO_ESCUCHA_FS 8090
#define PUERTO_ESCUCHA_MEM 8085
#define IP_LOCAL "127.0.0.1"



int escuchar(int puerto);
int conectarseA(char *ip, int puerto);
int aceptarConexion(int socketEscucha);
int enviarMensaje(char* mensaje, int socketDestino);
char* recibirMensaje(int socketCliente);


#endif
