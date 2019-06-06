/*
 * memoria.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "funciones.h"

/*
	Header provisorio
	SELECT 	1
	INSERT 	2
	CREATE 	3
*/

int main(){

	int conectado = 0;
	t_request request_ingresada;

	t_registro registro;
	int tamano_registro = sizeof(registro.key) + sizeof(registro.timestamp) + MAX_VALUE;

	memoria = malloc(tamano_registro * 30);
	tabla_segmentos = list_create();
	memset(memoria,NULL,tamano_registro * 30); //inicializa la memoria en NULL

	prueba(memoria,tabla_segmentos);

	puerto = escuchar(PUERTO_ESCUCHA_MEM);
	// Creacion de hilo
	pthread_t servidorKernel;
	pthread_create(&servidorKernel,NULL,(void*)conexionKernel,(void*)servidor);

	system("clear");
	printf("------------ MEMORIA ----------------\n");

	char * linea;
	while(1) {
		linea = readline(">");
		if(linea)
			add_history(linea);
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}

		request_ingresada = gestionarSolicitud(linea);

		procesarRequest(memoria,tabla_segmentos,request_ingresada);

		liberarMemoriaRequest(request_ingresada);
		//free(request_ingresada.value);
		//free(request_ingresada.nombre_tabla);
		free(linea);
	}

	free(memoria);
	//close(conectado);
	close(servidor);

	return 0;
}
