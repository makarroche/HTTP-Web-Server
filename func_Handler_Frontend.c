#include "func_Handler_Frontend.h" 


//Funcion para capturar SIGCHLD

void capture_sig_child(int signal, siginfo_t *info, void *context) {

	int status;
	
	if(wait(&status) == -1){;
		SYSERROR(EXIT_FAILURE,"Error en el wait");
	}
	else{
		printf("Handling signal %d with function\n", signal);
	}
}

//Funcion para setear SIGCHLD
void set_signal (void *handler, int signal_type){

	// Estructura de sigaction
	struct sigaction action_data;
	
	// Set signal handler
	action_data.sa_sigaction = handler;
	
	// Use signal flags
	action_data.sa_flags = SA_SIGINFO;
	
	// Install signal action
	if (sigaction(signal_type, &action_data, NULL) == -1) {
		perror(strerror(errno));
		exit(EXIT_FAILURE);
	}
} 


