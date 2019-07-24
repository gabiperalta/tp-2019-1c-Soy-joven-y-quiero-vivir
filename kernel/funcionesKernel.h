/*
 * funcionesKernel.h
 *
 *  Created on: 12 jun. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESKERNEL_H_
#define FUNCIONESKERNEL_H_


#include "kernel.h"

typedef struct{

	uint8_t id;
	char* nuevaRequestString;

}t_nueva_request;

/*
typedef struct{

	uint8_t tam_nombre_tabla;
	char* nombre_tabla;
	uint8_t tipo_consistencia;
	uint16_t numero_particiones;
	uint64_t compaction_time;

}t_tabla;
*/

void procesarRequest(uint8_t id,char* requestString);
void atenderNuevos();
void atenderListos();
void ejecutar(t_queue* script);
void crearEstructura(t_nueva_request* request);
//t_queue* crearEstructura(char * pathArchivo);
t_queue* leerArchivo(char * pathArchivo);
char* crearRequestString(char* requestLeido);
void leerArchivoConfig();
void enviarJournal(char* ip, int puerto);
void procesoGossiping();
void agregarMemoria(int idMemoria, uint8_t tipoConsistencia);
t_memoria* obtenerMemoria(char* nombreTabla);
void actualizarMetadata();
void recibirMetadata(int recibirCantidad,t_response response,int servidor);
t_response* buscarTabla(char* nombreTabla);


//t_tabla* buscarTabla(char* nombreTabla);
//void agregarTabla(t_response tablaRecibida);

#endif /* FUNCIONESKERNEL_H_ */
