/*
 * memoria.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "memoria.h"
#include <commons/string.h>
#include <commons/config.h>


int gestionarFuncionKernel(char* solicitud){
	char** spliteado = string_split(solicitud, " ");


	string_to_upper(spliteado[0]);


	if(!strcmp(spliteado[0], "SELECT")){
		printf("---select\n");
	}

	else if(!strcmp(spliteado[0], "INSERT")){
		printf("---insert\n");
	}

	else if(!strcmp(spliteado[0], "CREATE")){
		printf("---create\n");
	}

	else if(!strcmp(spliteado[0], "DESCRIBE")){
		printf("---describe\n");
	}

	else if(!strcmp(spliteado[0], "DROP")){
		printf("---drop\n");
	}

	else if(!strcmp(spliteado[0], "JOURNAL")){
		printf("---journal\n");
	}

	else{
		printf("La función no es correcta\n");
	}

	return 0;
}

void consola(){
	char * linea;
	while(1) {
		linea = readline(">");
		if (!linea){
			break;
		}
		gestionarFuncionKernel(linea);
		free(linea);
	}
}

int main(){

	/* // Crea un puntero al archivo de configuracion.
	t_config* archivoDeConfiguracion = config_create("Config.bin");
	// Devuelve el valor entero de una key (en este caso, PUERTO)
	int miPuerto = config_get_int_value(archivoDeConfiguracion, "PUERTO");

	int puerto = escuchar(miPuerto);
	*/

	int puerto = escuchar(PUERTO_ESCUCHA_MEM); // PUERTO_ESCUCHA_MEN = 8085
	int cantidadDeConexiones = 0;
	int conectado = 0;

	printf("------------PRUEBA MEMORIA----------------\n");
	printf("Ingrese la cantidad de conexiones: ");
	scanf ("%d", &cantidadDeConexiones);

	//Para realizar una conexion, en la terminal hay que ingresar: nc 127.0.0.1 8085
	//luego podemos ingresar una linea de texto. Cuando se presiona Enter, se finaliza
	//la conexion y se muestra la linea ingresada en la consola

	//Esta conexion la realizaremos la cantidad de veces que ingresamos al comienzo
	//del programa

	while(cantidadDeConexiones > 0){
		conectado = aceptarConexion(puerto);
		recibirMensaje(conectado);
		close(conectado);
		cantidadDeConexiones--;
	}


//		aceptarConexion(puertoLoco);
//		printf("Se conectó\n");
//		send(puertoLoco, "Hola", 5, 0);
//		send(puertoLoco, ":)", 3, 0);
//		recibirMensaje(puertoLoco);
//		close(puertoLoco);
	return 0;
}
