/*
 * funciones.c
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#include "funciones.h"
//#include "segmentacionPaginada.h"

t_request gestionarFuncionKernel(char* solicitud){
	char** spliteado = string_split(solicitud, " ");
	t_request request;
	request.value = malloc(MAX_VALUE);

	string_to_upper(spliteado[0]);

	if(!strcmp(spliteado[0], "SELECT")){
		printf("---select\n");
		request.header = 1;
		request.tam_nombre_tabla = strlen(spliteado[1]) + 1;
		request.nombre_tabla = malloc(request.tam_nombre_tabla);
		strcpy(request.nombre_tabla,spliteado[1]);
		request.key = atoi(spliteado[2]);
	}

	else if(!strcmp(spliteado[0], "INSERT")){
		printf("---insert\n");
		request.header = 2;
		request.tam_nombre_tabla = strlen(spliteado[1]) + 1;
		request.nombre_tabla = malloc(request.tam_nombre_tabla);
		strcpy(request.nombre_tabla,spliteado[1]);
		request.key = atoi(spliteado[2]);
		//request.key = strtol(spliteado[2],0); //probar si funciona
		strcpy(request.value,spliteado[3]);
	}

	else if(!strcmp(spliteado[0], "CREATE")){
		printf("---create\n");
		request.header = 3;
		request.tam_nombre_tabla = strlen(spliteado[1]) + 1;
		request.nombre_tabla = malloc(request.tam_nombre_tabla);
		strcpy(request.nombre_tabla,spliteado[1]);
		request.tipo_consistencia = atoi(spliteado[2]);
		request.numero_particiones = atoi(spliteado[3]);
		request.compaction_time = atoi(spliteado[4]);


		//CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
	}

	else if(!strcmp(spliteado[0], "DESCRIBE")){
		printf("---describe\n");
		request.header = 4;// fijarse bien que numero es
		request.tam_nombre_tabla = strlen(spliteado[1]) + 1;
		request.nombre_tabla = malloc(request.tam_nombre_tabla);
		strcpy(request.nombre_tabla,spliteado[1]);
		//ACA podrian pasarnos solo la palabra describe !!
	}

	else if(!strcmp(spliteado[0], "DROP")){
		printf("---drop\n");
		request.header = 5;
		request.tam_nombre_tabla = strlen(spliteado[1]) + 1;
		request.nombre_tabla = malloc(request.tam_nombre_tabla);
		strcpy(request.nombre_tabla,spliteado[1]);
	}

	else if(!strcmp(spliteado[0], "JOURNAL")){
		printf("---journal\n");
		request.header = 6;
	}

	else{
		printf("La función no es correcta\n");
	}

	return request;
}

void consola(){

	char * linea;
	while(1) {
		linea = readline(">");
		if(linea)
			add_history(linea);
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}
		gestionarFuncionKernel(linea);
		free(linea);
	}
}

void enviarRequestFileSystem(t_request request){
	int posicion = 0;

	int tamano_buffer;
	void* buffer;

	int servidor = conectarseA(IP_LOCAL, 40904);//PUERTO_ESCUCHA_FS

	switch(request.header){
		case 1: //SELECT


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

			send(servidor,buffer,tamano_buffer,0);

			break;
		case 4://DESCRIBE


			break;
		case 5://DROP

			break;
		case 6://JOURNAL
			break;
	}

	close(servidor);
	free(buffer);

}

t_request recibirRequestKernel(int socketCliente){

	int bytesRecibidos;
	t_request request;

	void* buffer = malloc(1000);

	bytesRecibidos = recv(socketCliente, buffer, sizeof(request.header), 0);

	if(bytesRecibidos <= 0) {
		perror("Error al recibir paquete");
	}

	memcpy(&request.header,buffer,sizeof(request.header));

	switch(request.header){
		case 1: //SELECT

			recv(socketCliente, buffer, sizeof(request.key), 0);
			memcpy(&request.key,buffer,sizeof(request.key));

			recv(socketCliente, buffer, sizeof(request.tam_nombre_tabla), 0);
			memcpy(&request.tam_nombre_tabla,buffer,sizeof(request.tam_nombre_tabla));

			recv(socketCliente, buffer, request.tam_nombre_tabla, 0);
			request.nombre_tabla = malloc(request.tam_nombre_tabla);
			memcpy(request.nombre_tabla,buffer,request.tam_nombre_tabla);

			break;
		case 2: //INSERT

			recv(socketCliente, buffer, sizeof(request.key), 0);
			memcpy(&request.key,buffer,sizeof(request.key));

			recv(socketCliente, buffer, sizeof(request.tam_nombre_tabla), 0);
			memcpy(&request.tam_nombre_tabla,buffer,sizeof(request.tam_nombre_tabla));

			recv(socketCliente, buffer, request.tam_nombre_tabla, 0);
			request.nombre_tabla = malloc(request.tam_nombre_tabla);
			memcpy(request.nombre_tabla,buffer,request.tam_nombre_tabla);

			recv(socketCliente, buffer, MAX_VALUE, 0);
			request.value = malloc(MAX_VALUE);
			memcpy(request.value,buffer,MAX_VALUE);

			break;
		case 3://CREATE
			break;
		case 4://DESCRIBE
			recv(socketCliente, buffer, sizeof(request.tam_nombre_tabla), 0);
			memcpy(&request.tam_nombre_tabla,buffer,sizeof(request.tam_nombre_tabla));

			recv(socketCliente, buffer, request.tam_nombre_tabla, 0);
			request.nombre_tabla = malloc(request.tam_nombre_tabla);
			memcpy(request.nombre_tabla,buffer,request.tam_nombre_tabla);

			break;
		case 5://DROP
			recv(socketCliente, buffer, sizeof(request.tam_nombre_tabla), 0);
			memcpy(&request.tam_nombre_tabla,buffer,sizeof(request.tam_nombre_tabla));

			recv(socketCliente, buffer, request.tam_nombre_tabla, 0);
			request.nombre_tabla = malloc(request.tam_nombre_tabla);
			memcpy(request.nombre_tabla,buffer,request.tam_nombre_tabla);
			break;
		case 6://JOURNAL
			break;
	}

	free(buffer);

	return request;
}

t_request armarInsert(char* nombreTabla, uint16_t key, char* value){

	t_request insert;

	strcpy(insert.nombre_tabla,nombreTabla);
	insert.key = key;
	strcpy(insert.value,value);

	return insert;
}

void prueba(void* memoria,t_list* tabla_segmentos){

	t_segmento* segmento_retornado;
	t_registro registro;

	registro.key = 16;
	registro.timestamp = 237;
	registro.value = "Espero que funcione";

	list_add(tabla_segmentos,crearSegmento("TABLA1"));
	segmento_retornado = (t_segmento*)list_get(tabla_segmentos,0);

	list_add(segmento_retornado->tabla_pagina,crearPagina(list_size(segmento_retornado->tabla_pagina),0,memoria,registro));

}

void procesarRequest(void* memoria,t_list* tabla_segmentos,t_request request){
	t_segmento* segmento_encontrado;
	t_pagina* pagina_encontrada;
	char* valueObtenido = malloc(MAX_VALUE);
	int timestampObtenido;
	t_registro registroNuevo;
	t_pagina* pagina_nueva;

	switch(request.header){
		case 1://SELECT

			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);
			pagina_encontrada = buscarPagina(segmento_encontrado->tabla_pagina,request.key,memoria);

			if(pagina_encontrada != 0){
				valueObtenido = obtenerValue(pagina_encontrada->direccion);
				printf("%s\n",valueObtenido);
			}

			break;
		case 2://INSERT

			registroNuevo.key = request.key;
			registroNuevo.value = request.value;
			registroNuevo.timestamp = (unsigned)time(NULL);
			//registroNuevo.timestamp = getCurrentTime(); // probar con (unsigned)
			segmento_encontrado = buscarSegmento(tabla_segmentos,request.nombre_tabla);

			if (segmento_encontrado!= NULL){
				pagina_encontrada = buscarPagina(segmento_encontrado->tabla_pagina,registroNuevo.key,memoria);

				if(pagina_encontrada != NULL){
					//valueObtenido = obtenerValue(pagina_encontrada->direccion);
					timestampObtenido = obtenerTimestamp(pagina_encontrada->direccion);

					if(timestampObtenido < registroNuevo.timestamp){//se actualiza el value
						actualizarRegistro(pagina_encontrada, registroNuevo);
					}
					else if (timestampObtenido >= registroNuevo.timestamp){/*no hay que actualizar*/}
				}
				else if (pagina_encontrada == NULL){// si no la encuentra
					list_add(segmento_encontrado->tabla_pagina,crearPagina(list_size(segmento_encontrado->tabla_pagina),1,memoria,registroNuevo));

					//////////////////////
					//ver si hay espacio//
					//////////////////////
				}
			}
			else if (segmento_encontrado == NULL){ // no se encontro el segmento
				int posicionSegmentoNuevo;
				t_segmento* segmento_nuevo;

				posicionSegmentoNuevo = list_add(tabla_segmentos,crearSegmento(request.nombre_tabla));
				segmento_nuevo = (t_segmento*)list_get(tabla_segmentos,posicionSegmentoNuevo);
				list_add(segmento_nuevo->tabla_pagina,crearPagina(0,1,memoria,registroNuevo));
			}

			break;
		case 3://CREATE

			enviarRequestFileSystem(request);

			break;
		case 4://DESCRIBE
			break;
		case 5://DROP
			break;
		case 6://JOURNAL
			break;
	}

	free(valueObtenido);
}
