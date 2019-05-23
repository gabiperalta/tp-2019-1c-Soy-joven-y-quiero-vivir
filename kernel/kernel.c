/*
 * filesystem.c
 *
 *  Created on: 11 abr. 2019
 *      Author: utnso
 */

#include "kernel.h"


/*
 * Devuelve el tamaño de un archivo.
 */
int32_t tamanioDeArchivo(FILE* f) {
	int32_t previo = ftell(f);
	fseek(f, 0, SEEK_END);
	int32_t tamanio = ftell(f);
	fseek(f, previo, SEEK_SET);
	return tamanio;
}

/*	PARSEARLQL
 * Recibe un .LQL y retorna un array que en la primera posición
 * contiene la cantidad de comandos que hay, y en el resto los comandos.
 */
//void parsearLQL(FILE* f, char* puntero) {
//	char* auxiliar = malloc(sizeof(tamanioDeArchivo(f)));
//	fgets(auxiliar, 500, f );
//	if(strlen(auxiliar) > 1){
//		strcpy(puntero, auxiliar);
//	}
//	free(auxiliar);
//}


/* JOURNAL
 * Le envia a memoría el comando para realizar el JOURNAL.
 */
void journal() {
	printf("---select\n");
	int16_t servidor = conectarseA(IP_LOCAL, PUERTO_ESCUCHA_MEM);
	enviarMensaje(JOURNAL, servidor);
	close(servidor);

}


void parsearLQL(FILE* f) {
	int32_t tamanioArchivo = tamanioDeArchivo(f);
	char* auxiliar = malloc(100);

	printf("El tamaño del archivo es de: %i", tamanioArchivo);


	while(!feof(f)){
		fgets(auxiliar, 150, f);
		printf("%s\n", auxiliar);
	}
}


/*PARSEAR POR ESPACIO
 *
 */
char** parsear(char** cosa, char* cadena) {
	cosa = string_split(cadena, " ");
	return cosa;
}



int gestionarFuncionKernel(char* solicitud) {
	char** spliteado = string_split(solicitud, " ");

	if(!strcmp(spliteado[0], "SELECT")) {
		printf("---select\n");
		// int servidor = conectarseA(ipDeLaMemoria, puertoDeLaMemoria);
		int16_t servidor = conectarseA(IP_LOCAL, PUERTO_ESCUCHA_MEM);
		enviarMensaje("Hola!", servidor);
		close(servidor);
//		enviarMensaje(spliteado[1], PUERTO_ESCUCHA_MEM); //IMPLEMENTACIÓN DE ENVIAR MENSAJE DE PRUEBA
	}

	else if(!strcmp(spliteado[0], "INSERT")) {
		printf("---insert\n");
	}

	else if(!strcmp(spliteado[0], "CREATE")) {
		printf("---create\n");
	}

	else if(!strcmp(spliteado[0], "DESCRIBE")) {
		printf("---describe\n");
		char* cadena = "SELECT eso FROM tabla";
		char** cosa = malloc(sizeof(cadena));


		cosa = parsear(cosa, cadena);
		printf("%s\n", cosa[0]);
		printf("%s\n", cosa[1]);
		printf("%s\n", cosa[2]);

		printf("%s\n", cosa[3]);


	}

	else if(!strcmp(spliteado[0], "DROP")) {
		printf("---drop\n");
	}

	else if(!strcmp(spliteado[0], "JOURNAL")) {
		printf("---journal\n");
		journal();
	}

	else if(!strcmp(spliteado[0], "ADD")) {
		printf("---add\n");
	}

	else if(!strcmp(spliteado[0], "RUN")) {
		printf("---run\n");
		FILE* archivo = fopen("programa.lql", "r");
		parsearLQL(archivo);

	}

	else if(!strcmp(spliteado[0], "METRICS")) {
		printf("---metrics\n");
	}

	else {
		printf("La función no es correcta\n");
	}

	return 0;



}


int main() {
	char * linea;
	while(1) {
		linea = readline(">");
		if (!linea) {
			break;
		}
		gestionarFuncionKernel(linea);
		free(linea);
	}

	return 0;
}

