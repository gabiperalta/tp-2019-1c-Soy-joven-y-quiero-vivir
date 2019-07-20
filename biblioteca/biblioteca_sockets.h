#ifndef BIBLIOTECA_SOCKETS_H_
#define BIBLIOTECA_SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "biblioteca.h"

#define PUERTO_ESCUCHA_FS 8090
#define PUERTO_ESCUCHA_MEM 36263
#define IP_LOCAL "127.0.0.1"


int escuchar(int puerto);
int conectarseA(char *ip, int puerto);
int aceptarConexion(int socketEscucha);
int enviarMensaje(char* mensaje, int socketDestino);
char* recibirMensaje(int socketCliente);
void enviarRequest(int servidor, t_request request);
void enviarResponse(int cliente,t_response response);
void enviarCantidadDeDescribes(int cliente,uint8_t cantidadDeDescribes);
void enviarCantidadDeJournal(int cliente,uint8_t cantidadDeJournal);
t_request recibirRequest(int servidor);
t_response recibirResponse(int servidor);
void recibirResponseDescribes(t_list* listaDeResponseDescribe, int servidor);

#endif
