#include"utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	// assert(!"no implementado!");

	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int sererr = getaddrinfo(NULL, PUERTO, &hints, &servinfo);

	if (sererr == -1){
		error_show("No se pudo obtener información del servidor");
		abort();
	}

	// Creamos el socket de escucha del servidor

	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	// Usamos setsockopt para poder aceptar varias conexiones al mismo puerto al bajar y levantar de nuevo el server
	int optval = 1;
	int err = setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int));

	// Asociamos el socket a un puerto
	err = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes
	// EL segundo parámetro son la cantidad de conexiones que puede mantener.
	err = listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	// assert(!"no implementado!");

	// Aceptamos un nuevo cliente
	int socket_cliente;

	
	socket_cliente = accept(socket_servidor, NULL, NULL);


	return socket_cliente;
}

void * atender_cliente(void* fd_conexion_ptr){
	log_info(logger, "Soy un cliente que habla desde un thread!\n");

	int cliente_fd = *(int*)fd_conexion_ptr;
	free(fd_conexion_ptr);

	t_list* lista;

	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			list_destroy_and_destroy_elements(lista, free); // ¡Importante!
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			close(cliente_fd);
			return NULL;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
