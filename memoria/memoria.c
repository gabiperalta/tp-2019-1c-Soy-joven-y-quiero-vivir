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

	archivo_config = config_create("/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/memoria/memoria.config");
	tamano_memoria = obtenerTamanioMemo();
	puerto_escucha_memoria = obtenerPuertoConfig();

	printf("%d\n",tamano_memoria);
	printf("%d\n",puerto_escucha_memoria);

	int conectado = 0;
	t_request request_ingresada;

	t_registro registro;
	int tamano_registro = sizeof(registro.key) + sizeof(registro.timestamp) + MAX_VALUE;

	memoria = malloc(tamano_memoria); //antes estaba tamano_registro * 60
	tabla_segmentos = list_create();
	memset(memoria,NULL,tamano_memoria); //inicializa la memoria en NULL, antes estaba tamano_registro * 60

	prueba(memoria,tabla_segmentos);

	puerto = escuchar(puerto_escucha_memoria); //antes estaba PUERTO_ESCUCHA_MEM

	// Creacion de hilo
	pthread_t servidorKernel;
	pthread_create(&servidorKernel,NULL,(void*)conexionKernel,NULL);
	pthread_detach(servidorKernel);

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
	//close(servidor);
	close(puerto);
	config_destroy(archivo_config);

	return 0;
}
