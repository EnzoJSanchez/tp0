#include "server.h"

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");

	while (1) {
     pthread_t thread;
     int *fd_conexion_ptr = malloc(sizeof(int));
     *fd_conexion_ptr = accept(server_fd, NULL, NULL);
		 log_info(logger, "Se conecto un cliente!\n");
     pthread_create(&thread,
                    NULL,
                    (void*) atender_cliente,
                    fd_conexion_ptr);
     pthread_detach(thread);
	}

	return EXIT_SUCCESS;
}


