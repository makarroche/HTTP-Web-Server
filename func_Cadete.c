#include "func_Cadete.h"



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

