#include "client.h"


t_log* iniciar_logger(){
	t_log* logger = log_create("tp0.log", "TP0", true, LOG_LEVEL_INFO);
	return logger;
};

t_config* iniciar_config(){
	t_config* config = config_create("cliente.config");
	return config;
};

void leer_consola(t_log* logger){
	char* console_value;
	while (1){
		console_value = readline(">");

		if (strcmp(console_value, "") == 0 || strcmp(console_value, "exit") == 0) {
            free(console_value);
        break;
        }
		
		log_info(logger, "Line: %s", console_value);
		free(console_value);
	}	
};



int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;


	/* ---------------- LOGGING ---------------- */



	logger = iniciar_logger();

	if (logger == NULL){
		fprintf(stderr, "Nose pudo crear el logger\n");
		return EXIT_FAILURE;
	};


	log_info(logger, "Soy un log");
	

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	
	config = iniciar_config();

	if (config == NULL){
		log_error(logger, "No se puedo abrir el archivo de configuración.");
		log_destroy(logger);
		return EXIT_FAILURE;
	};


	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	// Loggeamos el valor de config


	log_info(logger, "IP: %s", ip);
	log_info(logger, "PUERTO: %s", puerto);
	log_info(logger, "VALOR: %s", valor);


	/* ---------------- LEER DE CONSOLA ---------------- */	
	// leer_consola(logger);


	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje

	enviar_mensaje("YA FUNCIONAAA", conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}



void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	

	while (1){
		leido = readline(">");

		if (strcmp(leido, "") == 0 || strcmp(leido, "exit") == 0) {
            free(leido);
        	break;
        }
		int tamanio = strlen(leido) + 1; 
		agregar_a_paquete(paquete, leido, tamanio);
		free(leido);
	}	

	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	log_destroy(logger);
	config_destroy(config);
	sleep(1);
	liberar_conexion(conexion);
}
