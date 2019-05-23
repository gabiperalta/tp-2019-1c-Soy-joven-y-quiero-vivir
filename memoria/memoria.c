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
*/

int main(){
	int iniciar;
	int conectado = 0;
	t_request request_ingresada;

	t_registro registro;
	int tamano_registro = sizeof(registro.key) + sizeof(registro.timestamp) + 30;

	void* memoria = malloc(tamano_registro * 10);

	t_list* tabla_segmentos = list_create();


	//printf("Hasta aca funciona\n");

	prueba(memoria,tabla_segmentos);

	printf("------------ MEMORIA ----------------\n");
	printf("Modo de ejecucion:\n");
	printf("Ingresar por consola = 1\n");
	printf("Recibir del kernel   = 2\n--->");
	scanf ("%d", &iniciar);
	//system("clear");

	if(iniciar == 1){
		//consola(); //escribir "exit" para salir

		char * linea;
		while(1) {
			linea = readline(">");
			if(linea)
				add_history(linea);
			if(!strncmp(linea, "exit", 4)) {
				free(linea);
				break;
			}

			request_ingresada = gestionarFuncionKernel(linea);

			procesarRequest(memoria,tabla_segmentos,request_ingresada);

			free(linea);
		}
	}
	else{
		int puerto = escuchar(PUERTO_ESCUCHA_MEM); // PUERTO_ESCUCHA_MEM = 36263
		while(iniciar > 0){ // cancelar con CTRL + C
			conectado = aceptarConexion(puerto);
			recibirPaquete(conectado);
			close(conectado);

			iniciar--;
		}
	}



	return 0;
}
