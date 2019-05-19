/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

// voy a hacer un commit de prueba.

#include "filesystem.h"
#include "bibliotecaLFS.h"
#include <commons/string.h>
#include <time.h>



int gestionarFuncionFilesystem(char* solicitud) {
	char** spliteado = string_split(solicitud, " ");

	//printf("%s", spliteado[0]);


	if(!strcmp(spliteado[0], "SELECT")) {
		//printf("---select\n");

		selectLFS(spliteado[1], spliteado[2]);
	}

	else if(!strcmp(spliteado[0], "INSERT")) {
		//printf("---insert\n");
		if(string_length(spliteado) == 4){
			insertLFS(spliteado[1], spliteado[2], spliteado[3], spliteado[4]);
		}
		else{
			insertLFS(spliteado[1], spliteado[2], spliteado[3], "USE_TIMESTAMP");
		//														^---- como lo paso a string
		}
	}

	else if(!strcmp(spliteado[0], "CREATE")) {
		//printf("---create\n");

		createLFS(spliteado[1], spliteado[2], spliteado[3], spliteado[4]);
	}

	else if(!strcmp(spliteado[0], "DESCRIBE")) {
		//printf("---describe\n");
		if(string_length(spliteado) > 1){
			describeLSF(spliteado[1]);
		}
		else{
			describeLSF("DEFAULT");
		}
	}

	else if(!strcmp(spliteado[0], "DROP")) {
		//printf("---drop\n");
		dropLSF(spliteado[1]);
	}

	else {
		printf("La funcion no es correcta\n");
	}

	return 0;
}

int main() {

	char * linea;
	while(1) {
		linea = readline(">"); //    CREATE TABLA1 SC 3 60000
		if (!linea) {
			break;
		}
		gestionarFuncionFilesystem(linea);
		free(linea);
	}

	return 0;
}





// HACER LA FUNCION GENERICA PARA EL MANEJO DE ARCHIVOS
/*
// Crea un puntero al archivo de configuracion.
		t_config* archivoDeConfiguracion = config_create("Config.bin");
		// Devuelve el valor entero de una key (en este caso, PUERTO)
		int puertoDeLaMemoria = config_get_int_value(archivoDeConfiguracion, "PUERTO");
		char* ipDeLaMemoria = config_get_string_value(archivoDeConfiguracion, "IP_MEMORIA");
*/




