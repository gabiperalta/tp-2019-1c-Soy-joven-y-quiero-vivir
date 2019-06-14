#include "servidor.h"
#include "biblioteca_sockets.h"

#define BACKLOG 5
#define PACKAGESIZE 1024

void inicializarServidor(){
	/*
	 *  ¿Quien soy? ¿Donde estoy? ¿Existo?
	 *
	 *  Estas y otras preguntas existenciales son resueltas getaddrinfo();
	 *
	 *  Obtiene los datos de la direccion de red y lo guarda en serverInfo.
	 *
	 */
	t_config* config = obtenerConfigDeFS();

	int PUERTO = config_get_int_value(config, "PUERTO_ESCUCHA");


	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(NULL, PUERTO, &hints, &serverInfo); // Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE


	/*
	 * 	Descubiertos los misterios de la vida (por lo menos, para la conexion de red actual), necesito enterarme de alguna forma
	 * 	cuales son las conexiones que quieren establecer conmigo.
	 *
	 * 	Para ello, y basandome en el postulado de que en Linux TODO es un archivo, voy a utilizar... Si, un archivo!
	 *
	 * 	Mediante socket(), obtengo el File Descriptor que me proporciona el sistema (un integer identificador).
	 *
	 */
	/* Necesitamos un socket que escuche las conecciones entrantes */
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	/*
	 * 	Perfecto, ya tengo un archivo que puedo utilizar para analizar las conexiones entrantes. Pero... ¿Por donde?
	 *
	 * 	Necesito decirle al sistema que voy a utilizar el archivo que me proporciono para escuchar las conexiones por un puerto especifico.
	 *
	 * 				OJO! Todavia no estoy escuchando las conexiones entrantes!
	 *
	 */
	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	/*
	 * 	Ya tengo un medio de comunicacion (el socket) y le dije por que "telefono" tiene que esperar las llamadas.
	 *
	 * 	Solo me queda decirle que vaya y escuche!
	 *
	 */



	while(1){
	listen(listenningSocket, BACKLOG);	// IMPORTANTE: listen() es una syscall BLOQUEANTE.
	/*
	 * 	El sistema esperara hasta que reciba una conexion entrante...
	 * 	...
	 * 	...
	 * 	BING!!! Nos estan llamando! ¿Y ahora?
	 *
	 *	Aceptamos la conexion entrante, y creamos un nuevo socket mediante el cual nos podamos comunicar (que no es mas que un archivo).
	 *
	 *	¿Por que crear un nuevo socket? Porque el anterior lo necesitamos para escuchar las conexiones entrantes. De la misma forma que
	 *	uno no puede estar hablando por telefono a la vez que esta esperando que lo llamen, un socket no se puede encargar de escuchar
	 *	las conexiones entrantes y ademas comunicarse con un cliente.
	 *
	 *			Nota: Para que el listenningSocket vuelva a esperar conexiones, necesitariamos volver a decirle que escuche, con listen();
	 *				En este ejemplo nos dedicamos unicamente a trabajar con el cliente y no escuchamos mas conexiones.
	 *
	 */

	//crearHiloDeAtencion(listenningSocket);

	pthread_t hilo;
	struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);

	/*
	 * 	Ya estamos listos para recibir paquetes de nuestro cliente...
	 *
	 * 	Vamos a ESPERAR (ergo, funcion bloqueante) que nos manden los paquetes, y los imprimieremos por pantalla.
	 *
	 *	Cuando el cliente cierra la conexion, recv() devolvera 0.
	 */
	//char package[PACKAGESIZE];
	//int status = 1;		// Estructura que manjea el status de los recieve.

	printf("Cliente conectado. Esperando mensajes:\n");

	/*while (status != 0){
		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);
	}*/

	crearHiloDeAtencion(listenningSocket,socketCliente);

	}

	/*
	 * 	Terminado el intercambio de paquetes, cerramos todas las conexiones.
	 */

	close(listenningSocket);


	return;
}

void atenderRequest(int socketCliente){
	t_request request = recibirRequest(socketCliente);
	int tamanioBuffer;
	char* respuesta = malloc(100);
	char* consistencia = malloc(4);
	switch(request.header){
	case SELECT: // SELECT

		respuesta = selectLFS(request.nombre_tabla, string_itoa(request.key));
		tamanioBuffer = sizeof(respuesta);
		send(socketCliente, respuesta, tamanioBuffer, NULL);

		break;
	case INSERT: // INSERT

		insertLFS(request.nombre_tabla, string_itoa(request.key), request.value, string_itoa(request.timestamp));

		break;
	case CREATE: // CREATE

		switch(request.tipo_consistencia){
		case SC:
			consistencia = "SC";
			break;
		case SHC:
			consistencia = "SHC";
			break;
		case EC:
			consistencia = "EC";
		}

		createLFS(request.nombre_tabla, consistencia, string_itoa(request.numero_particiones), string_itoa(request.compaction_time));
		free(consistencia);
		break;
	case DESCRIBE: // DESCRIBE

		if( request.tam_nombre_tabla > 1){

			describeLSF(request.nombre_tabla);

		}
		else{

			describeLSF("DEFAULT");

		}

		break;
	case DROP: // DROP

		dropLSF(request.nombre_tabla);

		break;

	}
	liberarMemoriaRequest(request);
	return;
}


void crearHiloDeAtencion(int listenningSocket, int socketCliente){
	while((socketCliente=aceptarConexion(listenningSocket))!= 1){
		pthread_t hiloRequest;
		pthread_create(&hiloRequest,NULL,(void*)atenderRequest,socketCliente);\
		pthread_join(&hiloRequest, NULL);
		close(socketCliente);
	}
}

/*void crearHiloDeAtencion(listenningSocket){
	pthread_t hilo;
	pthread_create(&hilo, NULL, atenderCliente, NULL);
}

void atenderCliente(){

}*/
