#include "func_Handler_Frontend.h" 


int main (int argc, char **argv) {
	
	pid_t resultado_fork;
	SHM_Access axs;
	Socket_Access socket_axs;
	int new_socket;
	char derivar_a_hijo[DAD_TO_KID];

	// Instalar senal SIGCHLD
	set_signal(capture_sig_child, SIGCHLD);
	
	// Solicitar Shared Memory Management
	request_shared_memory(&axs, "shm_management", MYPROJECTKEY); 
	// Inicializar el segmento de memoria
	initialize_shm_management(axs);
	// Detach de Shared Memory Management
	detach_shared_memory(axs);
	
	// Seteo el Handler de la senial asesina total
	
	// Mientras no venga la senal para matar el sistema TOTAL va a quedar en escucha permanente

	// Crea socket con Cliente
	socket_axs = create_socket(_PORT_);
	
	while(1){
		// En escucha con Cliente
		new_socket = socket_escucha(socket_axs);
	
		// Crea hijo
		if ((resultado_fork = fork()) > 0) {
			// Estoy en el proceso padre
			printf("Estoy en el padre\n");

		} else if (resultado_fork == 0) {
			// Derivar conexion a hijo
			sprintf(derivar_a_hijo, "%d", new_socket);
			if (execl("./Handler_Frontend_Child", "./Handler_Frontend_Child", derivar_a_hijo, NULL) == -1) {
				SYSERROR(EXIT_FAILURE, "Error al ejecutar H_F_Child.\n");
			}

		} else {
			SYSERROR(EXIT_FAILURE, "Error al generar proceso hijo Handler Frontend Child.\n");
		}
	}
}

