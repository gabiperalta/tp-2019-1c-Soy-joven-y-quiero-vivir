#include "servidor.h"
#include "../biblioteca/biblioteca_sockets.h"

/*void inicializarServidor(){
	/*
	 *  ¿Quien soy? ¿Donde estoy? ¿Existo?
	 *
	 *  Estas y otras preguntas existenciales son resueltas getaddrinfo();
	 *
	 *  Obtiene los datos de la direccion de red y lo guarda en serverInfo.
	 *
	 */
/*	t_config* config = obtenerConfigDeFS();

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
/*	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	/*
	 * 	Perfecto, ya tengo un archivo que puedo utilizar para analizar las conexiones entrantes. Pero... ¿Por donde?
	 *
	 * 	Necesito decirle al sistema que voy a utilizar el archivo que me proporciono para escuchar las conexiones por un puerto especifico.
	 *
	 * 				OJO! Todavia no estoy escuchando las conexiones entrantes!
	 *
	 */
/*	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	/*
	 * 	Ya tengo un medio de comunicacion (el socket) y le dije por que "telefono" tiene que esperar las llamadas.
	 *
	 * 	Solo me queda decirle que vaya y escuche!
	 *
	 */

/*

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
/*
	pthread_t hilo;
	struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);
*/
/*	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);

	/*
	 * 	Ya estamos listos para recibir paquetes de nuestro cliente...
	 *
	 * 	Vamos a ESPERAR (ergo, funcion bloqueante) que nos manden los paquetes, y los imprimieremos por pantalla.
	 *
	 *	Cuando el cliente cierra la conexion, recv() devolvera 0.
	 */
	//char package[PACKAGESIZE];
	//int status = 1;		// Estructura que manjea el status de los recieve.
/*
	printf("Cliente conectado. Esperando mensajes:\n");
*/
	/*while (status != 0){
		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);
	}*//*

	crearHiloDeAtencion(listenningSocket,socketCliente);

	}*/

	/*
	 * 	Terminado el intercambio de paquetes, cerramos todas las conexiones.
	 *//*

	close(listenningSocket);


	return;
}*/

void inicializarServidorV2(){
	extern int socketDeEscucha;
	t_config* config = obtenerConfigDeFS();
	int puertoDeEscucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	socketDeEscucha = escuchar(puertoDeEscucha);
	int socketCliente;

	printf("socketDeEscucha: %i\n", socketDeEscucha);
	pthread_t hiloServidor;
	pthread_create(&hiloServidor,NULL,(void*)crearHiloDeAtencion, (void*)socketDeEscucha);
	pthread_detach(hiloServidor);

}


void atenderRequest(int socketCliente){
	t_request request = recibirRequest(socketCliente);
	int error;
	bool fueDescribeExitoso = false;
	nodo_memtable* respuestaSelect = malloc(sizeof(nodo_memtable));
	char* consistencia = malloc(4);
	t_response structRespuesta;
	t_list* respuestaDescribe;
	datos_metadata* datosMetadata = malloc(sizeof(datos_metadata));
	int cantidadDeDescribes;

	switch(request.header){
	case SELECT: // SELECT

		respuestaSelect = selectLFS(request.nombre_tabla, string_itoa(request.key));

		if(respuestaSelect == NULL){
			structRespuesta.header = ERROR_R;
			logError("Filesystem: Fallo un SELECT de un cliente porque no se encontro la key.");
		}
		else{
			structRespuesta.header = SELECT_R;
			structRespuesta.tam_value = strlen(respuestaSelect->value);
			structRespuesta.value = malloc(structRespuesta.tam_value);
			strcpy(structRespuesta.value, respuestaSelect->value);
			structRespuesta.timestamp = respuestaSelect->timestamp;

			logInfo("Filesystem: Se realizo un SELECT de un cliente.");

		}


		break;
	case INSERT: // INSERT

		error = insertLFS(request.nombre_tabla, string_itoa(request.key), request.value, string_itoa(request.timestamp));

		if( error ){
			structRespuesta.header = ERROR_R;
			logError("Filesystem: Fallo un INSERT de un cliente porque la tabla no existe en el LFS.");
		}
		else{
			structRespuesta.header = INSERT_R;
			logInfo("Filesystem: Se realizo un INSERT de un cliente.");
		}

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

		error = createLFS(request.nombre_tabla, consistencia, string_itoa(request.numero_particiones), string_itoa(request.compaction_time));
		free(consistencia);

		if(error){
			structRespuesta.header = ERROR_R;
			logError("Filesystem: Fallo un CREATE de un cliente porque ya existe la tabla en el LFS.");
		}
		else{
			structRespuesta.header = CREATE_R;
			logInfo("Filesystem: Se realizo un CREATE de un cliente.");
		}

		break;
	case DESCRIBE: // DESCRIBE

		fueDescribeExitoso = true;

		if( request.tam_nombre_tabla > 1){

			respuestaDescribe = describeLSF(request.nombre_tabla);

		}
		else{

			respuestaDescribe = describeLSF("DEFAULT"); // TODO hacer un int que diga si fue un describe, en tal caso ignorar el enviar response de abajo y hacer un for para enviarlos.

		}

		cantidadDeDescribes = respuestaDescribe->elements_count;

		if(cantidadDeDescribes < 1){
			structRespuesta.header = ERROR_R;
			logError("Filesystem: Fallo un DESCRIBE de un cliente.");
			fueDescribeExitoso = false;
		}else{
			logInfo("Filesystem: Se realizo un DESCRIBE de un cliente.");
		}

		break;
	case DROP: // DROP

		error = dropLSF(request.nombre_tabla);

		if(error){
			structRespuesta.header = ERROR_R;

			logError("Filesystem: Fallo un DROP de un cliente porque la tabla no existe.");
		}
		else{
			structRespuesta.header = DROP_R;
			logInfo("Filesystem: Se realizo un DROP de un cliente.");
		}

		break;

	}
	if(!fueDescribeExitoso){
		enviarResponse(socketCliente, structRespuesta);
	}
	else{
		enviarCantidadDeDescribes(socketCliente, cantidadDeDescribes);
		for(int i = 0; i < cantidadDeDescribes; i++){
			datosMetadata = list_get(respuestaDescribe, i);
			structRespuesta.header = DESCRIBE_R;
			structRespuesta.tam_nombre_tabla = strlen(datosMetadata->nombreTabla);
			structRespuesta.nombre_tabla = malloc(structRespuesta.tam_nombre_tabla);
			strcpy(structRespuesta.nombre_tabla, datosMetadata->nombreTabla);
			structRespuesta.compaction_time = structRespuesta.tam_value;
			if(strcmp(datosMetadata, "SC")){
				structRespuesta.tipo_consistencia = 1;
			}else if(strcmp(datosMetadata, "SHC")){
				structRespuesta.tipo_consistencia = 2;
			}else if(strcmp(datosMetadata, "EC")){
				structRespuesta.tipo_consistencia = 3;
			}
			enviarResponse(socketCliente, structRespuesta);
			free(structRespuesta.nombre_tabla);
		}
	}
	liberarMemoriaRequest(request);
	close(socketCliente);
	return;
}


void crearHiloDeAtencion(int listenningSocket){
	int socketCliente;
	while((socketCliente=aceptarConexion(listenningSocket))!= 1){
		pthread_t hiloRequest;
		pthread_create(&hiloRequest,NULL,(void*)atenderRequest, (void*)socketCliente);
		pthread_detach(hiloRequest);

	}
}

