/*
 * Copyright (C) 2012 Sistemas Operativos - UTN FRBA. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Spock.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>

static char* mensajeRandom();

t_spock* spock_create() {
	t_spock* spock = malloc(sizeof(t_spock));
	spock->edad = 40;
	spock->nombre = strdup("Roberto Spock");

	spock->mascota = mascota_crear("Babu", true, 5);
	spock->villanos = list_create();

	list_add(spock->villanos, villano_create("Borg Queen", 34));
	list_add(spock->villanos, villano_create("Locotus", 20));
	list_add(spock->villanos, villano_create("Dukat", 67));

	char* mensaje = mensajeRandom();
	spock->mision = mision_crear(mensaje);

	return spock;
}

void spock_destroy(t_spock* spock) {
	if (spock != NULL) {
		free(spock->nombre);
		mascota_destroy(spock->mascota);
		mision_destroy(spock->mision);
		list_destroy_and_destroy_elements(spock->villanos, (void*) villano_destroy);
		free(spock);
	}
}

int serializar_mision(t_mision* mision, void* stream){
	int size_stream = mision->longitud_info + sizeof(int);
	stream = malloc(size_stream);

	int offset = 0;

	memcpy(stream + offset,&mision->longitud_info, sizeof(int));
	offset+=sizeof(int);
	memcpy(stream + offset,mision->info_codificada,mision->longitud_info);
	offset+=sizeof(int);
	return size_stream;
}

int serializar_mascota(t_mascota* mascota, void* stream){
	int size_stream = strlen(mascota->apodo) + 1 + sizeof(int) + sizeof(int) + sizeof(bool); //son dos sizeof int para el tamaño del apodo
	stream = malloc(size_stream);
	int tamanio_apodo = strlen(mascota->apodo) + 1;
	int offset = 0;

	memcpy(stream + offset, &tamanio_apodo, sizeof(int));
	offset+=sizeof(int);
	memcpy(stream + offset, mascota->apodo, tamanio_apodo);
	offset+=tamanio_apodo;
	memcpy(stream + offset, &mascota->edad, sizeof(int));
	offset+=sizeof(int);
	memcpy(stream + offset, &mascota->da_vueltas, sizeof(bool));
	offset+=sizeof(bool);

	return size_stream;

	/*
	 * la serializacion en este caso consiste en asignar los campos del struct t_mascota
	 * a un puntero void* en un orden de empaquetado tal que quede compacto para poder
	 * enviarse por el metodo TPC/IP Sockets mediante send/recv. Al principio inicializo un
	 * offset para indicar el desplazamiento dentro del puntero creado con respecto a lo
	 * incluido en la serializacion.
	 *
	 * size_stream: es el tamaño total de la serializacion. Es el puntero char* + \0, dos int
	 * (edad y el tamaño del puntero), y un tamaño bool.
	 *
	 *	Se asigna primero el tamanio_apodo antes que el puntero para saber cuantos bytes ocupa
	 *	el puntero siguiente.
	 *
	 *	Esta forma de serializar no es la unica, tambien podria ser al reves. Solamente se debe
	 *	respetar el orden de empaquetado y desempaquetado.
	 */

}

int serializar_villanos(t_list* villanos, void* stream){
	int cantidad_villanos = list_size(villanos);
	int size_stream = sizeof(uint16_t)*cantidad_villanos + 26*cantidad_villanos; // campo edad * tamaño lista + 26 del string nombre * tamaño lista
	stream = malloc(size_stream);									   // tambien se podria hacer sizeof(t_villano) ya que no tiene punteros

	int offset = 0;
	t_villano* villano_aux;

	memcpy(stream + offset, &cantidad_villanos, sizeof(int));
	offset+=sizeof(int);
	for(int i=0;i<cantidad_villanos;i++){
		villano_aux = list_get(villanos,i);
		memcpy(stream + offset, villano_aux->nombre, 26);
		offset+=26;
		memcpy(stream + offset,&villano_aux->edad, sizeof(int));
		offset+= sizeof(int);
	}
	return size_stream;
}


void* serializar_spock(t_spock* spock, int* tamanio){
	//t_buffer* buffer = malloc(sizeof(buffer));



	void *mision=NULL, *mascota=NULL, *villanos=NULL;
	int mision_size = serializar_mision(spock->mision, mision);
	int mascota_size = serializar_mascota(spock->mascota, mascota);
	int villanos_size = serializar_villanos(spock->villanos, villanos);



	int spock_size = mision_size + mascota_size + villanos_size + strlen(spock->nombre) + 1 + sizeof(int);

	puts("Hasta aca llegue");

	memcpy(tamanio,&spock_size,sizeof(int));


	puts("Hasta aca llegue2");
	printf("%d", spock_size);
	void* stream = malloc(spock_size);
	puts("Hasta aca llegue3");

	int offset=0;
	int tam_nombre = strlen(spock->nombre)+1;

	puts("Hasta aca llegue4");

	memcpy(stream + offset, &tam_nombre, sizeof(int));
	offset+=tam_nombre;
	memcpy(stream + offset, spock->nombre, tam_nombre);
	offset+=tam_nombre;
	memcpy(stream + offset, &spock->edad, sizeof(int));
	offset+=sizeof(int);




	//memcpy(stream + offset, &mision_size, sizeof(int));
	//offset+=sizeof(int);
	memcpy(stream + offset, &mision, mision_size);
	offset+=mision_size;

	//memcpy(stream + offset, &mascota_size, sizeof(int));
	//offset+=sizeof(int);
	memcpy(stream + offset, &mascota, mascota_size);
	offset+=mascota_size;

	//memcpy(stream + offset, &villanos_size, sizeof(int));
	//offset+=sizeof(int);
	memcpy(stream + offset, &villanos, villanos_size);
	offset+=villanos_size;

	return stream;

}

void spock_enviar_a_mision(t_spock* spock, char* file_name) {
	FILE* file = fopen(file_name, "w");

	puts("Abre Archivo");

	int tam_spock_serializado;
	void* stream = serializar_spock(spock,&tam_spock_serializado);
	printf("%d", tam_spock_serializado);
	fwrite(stream, tam_spock_serializado, 1, file);
	fflush(file);
	fclose(file);
}

void deserializar_mascota(t_spock* spock, void* stream){
	t_mascota* mascota = malloc(sizeof(t_mascota));

	int tam_apodo;

	memcpy(&tam_apodo, stream, sizeof(int));
	stream+=sizeof(int);
	mascota->apodo = malloc(tam_apodo);
	memcpy(&mascota->apodo, stream, tam_apodo);
	stream+=tam_apodo;
	memcpy(&mascota->edad, stream, sizeof(int));
	stream+=sizeof(int);
	memcpy(&mascota->da_vueltas, stream, sizeof(bool));
	stream+=sizeof(bool);

	spock->mascota = mascota;

}

void deserializar_mision(t_spock* spock, void* stream){
	t_mision* mision = malloc(sizeof(t_mision));

	memcpy(&mision->longitud_info, stream, sizeof(int));
	mision->info_codificada = malloc(mision->longitud_info);
	memcpy(&mision->info_codificada, stream, mision->longitud_info);

	spock->mision=mision;

}

void deserializar_villanos(t_spock* spock, void* stream){
	t_list* villanos = list_create();

	int cantidad_villanos;
	t_villano* villano;
	memcpy(&cantidad_villanos, stream, sizeof(int));
	stream+=sizeof(int);
	for(int i=0;i<cantidad_villanos;i++){
		villano = malloc(sizeof(t_villano));
		memcpy(&villano->nombre, stream, 26);
		stream+=26;
		memcpy(&villano->edad, stream, sizeof(uint16_t));
		stream+=sizeof(uint16_t);
		list_add(villanos, villano);
	}

	spock->villanos=villanos;
}

t_spock* deserializar_spock(void* stream_original){
	t_spock* spock = malloc(sizeof(t_spock));

	int tam_apodo;

	void* stream = stream_original;

	//

	memcpy(&tam_apodo,stream, sizeof(int));
	stream+=sizeof(int);
	memcpy(spock->nombre, stream, tam_apodo);
	stream+=tam_apodo;
	memcpy(&spock->edad, stream, sizeof(int));
	stream+=sizeof(int);

//-------------------------------------------------------------------
	// DESERIALIZO MISION
	deserializar_mision(spock, stream);

//-------------------------------------------------------------------
	// DESERIALIZO MASCOTA
	deserializar_mascota(spock, stream);

//-------------------------------------------------------------------
	// DESERIALIZO VILLANOS
	deserializar_villanos(spock, stream);

	return spock;

}

t_spock* spock_volver_de_mision(char* file_name){
	FILE* file = fopen(file_name,"r");
	struct stat statSpock;
	stat(file_name,&statSpock);
	void* stream = malloc(statSpock.st_size);
	fread(stream,statSpock.st_size,1,file);
	return deserializar_spock(stream);
	//printf("ahhhhhhh!! no podemos recuperar a Spock\n");
	//return NULL;
}

void spock_es_igual(t_spock* spock, t_spock* otro_spock) {
	assert(strcmp(spock->nombre, otro_spock->nombre) == 0);
	assert(spock->edad == otro_spock->edad);

	assert(strcmp(spock->mascota->apodo, otro_spock->mascota->apodo) == 0);
	assert(spock->mascota->da_vueltas == otro_spock->mascota->da_vueltas);
	assert(spock->mascota->edad == otro_spock->mascota->edad);

	assert(strcmp(spock->mision->info_codificada, otro_spock->mision->info_codificada) == 0);
	assert(spock->mision->longitud_info == otro_spock->mision->longitud_info);

	int i;
	for (i = 0; i < list_size(spock->villanos); ++i) {
		t_villano* villano_enviado = list_get(spock->villanos, i);
		t_villano* villano_recibido = list_get(otro_spock->villanos, i);

		assert(strcmp(villano_enviado->nombre, villano_recibido->nombre) == 0);
		assert(villano_enviado->edad == villano_recibido->edad);
	}

}

void spock_print(t_spock* spock) {
	printf("Nombre: %s\n", spock->nombre);
	printf("Edad: %d\n", spock->edad);
	printf("Apodo de la mascota: %s\n", spock->mascota->apodo);
	printf("Mascota da vueltas?: %s\n", spock->mascota->da_vueltas ? "Si" : "No");
	printf("Edad de la mascota: %d\n", spock->mascota->edad);
	printf("Info Codificada: %s\n", spock->mision->info_codificada);
	printf("Longitud Info Codificada: %d\n", spock->mision->longitud_info);

	int i;
	printf("Villanos:\n");
	for (i = 0; i < list_size(spock->villanos); ++i) {
		t_villano* villano = list_get(spock->villanos, i);
		printf("Nombre Villano: %s\n", villano->nombre);
		printf("Edad del villano: %d\n", villano->edad);
	}

}

static char* mensajeRandom() {
	return "ABCDEFGHI";
}
