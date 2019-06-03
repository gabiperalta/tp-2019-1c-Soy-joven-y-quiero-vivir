#include "biblioteca_sockets.h"

#define MAX_CLIENTES 10



/*	ESCUCHAR
 * Recibe un puerto y es espera conexión en ese puerto.
 * Retorna -1 en caso de que ocurra un error.
 * Retorna el socket escuchado en caso de haber recibido una conexión.
 */


int escuchar(int puerto) {
	int socketDeEscucha;
	struct sockaddr_in direccionCliente;
	direccionCliente.sin_family = AF_INET;
	direccionCliente.sin_addr.s_addr = INADDR_ANY;
	direccionCliente.sin_port = htons(puerto);


	if((socketDeEscucha = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		printf("Error al crear el socket de escucha\n");
		return -1;
	}
	int activador = 1;
	if(setsockopt(socketDeEscucha, SOL_SOCKET, SO_REUSEADDR, (char*) &activador, sizeof(activador)) < 0) {
		printf("Error al usar setsockopt\n");
		return -1;
	}



	if(bind(socketDeEscucha, (struct sockaddr *) &direccionCliente, sizeof(direccionCliente)) < 0) {
		printf("Error al bindear\n");
		return -1;

	}

	if(listen(socketDeEscucha, MAX_CLIENTES) < 0) {
		printf("Error al escuchar\n");
		return -1;
	}
	return socketDeEscucha;
}

/*	CONECTARSEA
 * Recibe un IP y un puerto y se conecta al SV.
 * Retonar 0 en caso de no conectarse.
 * Retonar el ID del socket cliente.
 */

int conectarseA(char* ip, int puerto) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if(connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("No se pudo conectar\n");
		return 0;
	}
	return cliente;
}

/*ACEPTARCONEXION
 * Retorna 1 en caso de no poder conectarse.
 * Retorna el ID del socket que se conectó.
 */

int aceptarConexion(int socketEscucha) {
	int nuevoSocket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	if ((nuevoSocket = accept(socketEscucha, (struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
		return 1;
	}

	return nuevoSocket;
}


///*ENVIARMENSAJE
// *
// */

int enviarMensaje(char* mensaje, int socketDestino) {
	int envioCorrecto=0;
	int longitudMensaje = strlen(mensaje);
	envioCorrecto = send(socketDestino, mensaje, longitudMensaje + 1, 0);
	return 0;
}


/*RECIBIRMENSAJE
 *  Retorna 1 en caso de haber un error al recibir el mensaje.
 *  Retonar el contenido del mensaje.
 */

char* recibirMensaje(int socketCliente) {
	char* buffer = malloc(1000);

	int bytesRecibidos = recv(socketCliente, buffer, 1000, 0);

	if(bytesRecibidos <= 0) {
		perror("Error al recibir mensaje");
		return 1;
	}

	buffer[bytesRecibidos] = '\0';
	printf("Recibí: %s", buffer);

	return buffer;
}

void enviarRequest(int servidor,t_request request){
	int posicion = 0;

	int tamano_buffer;
	void* buffer;

	switch(request.header){
		case 1: //SELECT

			tamano_buffer = sizeof(request.key) + sizeof(request.header)
			+ sizeof(request.tam_nombre_tabla) + request.tam_nombre_tabla
			+ sizeof(request.tam_value) + request.value;
			buffer = malloc(tamano_buffer);

			memcpy(&buffer[posicion],&request.header,sizeof(request.header));
			posicion += sizeof(request.header);

			memcpy(&buffer[posicion],&request.key,sizeof(request.key));
			posicion += sizeof(request.key);

			memcpy(&buffer[posicion],&request.tam_nombre_tabla,sizeof(request.tam_nombre_tabla));
			posicion += sizeof(request.tam_nombre_tabla);

			memcpy(&buffer[posicion],request.nombre_tabla,request.tam_nombre_tabla);
			posicion += request.tam_nombre_tabla;

			memcpy(&buffer[posicion],&request.tam_value,sizeof(request.tam_value));
			posicion += sizeof(request.tam_value);

			request.value[strlen(request.value)] = '\0';
			memcpy(&buffer[posicion],request.value,30);

			break;
		case 2: //INSERT



			break;
		case 3://CREATE

			tamano_buffer = sizeof(request.header) + sizeof(request.tam_nombre_tabla) + request.tam_nombre_tabla
			+ sizeof(request.tipo_consistencia) + sizeof(request.numero_particiones) + sizeof(request.compaction_time);

			buffer = malloc(tamano_buffer);

			memcpy(&buffer[posicion],&request.header,sizeof(request.header));
			posicion += sizeof(request.header);

			memcpy(&buffer[posicion],&request.tam_nombre_tabla,sizeof(request.tam_nombre_tabla));
			posicion += sizeof(request.tam_nombre_tabla);

			memcpy(&buffer[posicion],request.nombre_tabla,request.tam_nombre_tabla);
			posicion += request.tam_nombre_tabla;

			memcpy(&buffer[posicion],&request.tipo_consistencia,sizeof(request.tipo_consistencia));
			posicion += sizeof(request.tipo_consistencia);

			memcpy(&buffer[posicion],&request.numero_particiones,sizeof(request.numero_particiones));
			posicion += sizeof(request.numero_particiones);

			memcpy(&buffer[posicion],&request.compaction_time,sizeof(request.compaction_time));



			break;
		case 4://DESCRIBE


			break;
		case 5://DROP

			break;
		case 6://JOURNAL

			break;
	}

	send(servidor,buffer,tamano_buffer,0);

	free(buffer);
}
