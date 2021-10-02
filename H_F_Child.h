#ifndef _FRONTEND_CHILD_
#define _FRONTEND_CHILD_

#include "includes.h"
#include "socket.h"
#include "shared_memory.h"

#define MAX_LENGTH_REQUEST 3000
#define MAX_LENGTH_RESPONSE 3000
#define NOT_IMPLEMENTED 5
#define BAD_REQUEST 6
#define NOT_SUPPORTED 7
#define FORMAT_OK 0
#define SECOND_TOKEN 0
#define THIRD_TOKEN 1
#define FOURTH_TOKEN 2
#define FIFTH_TOKEN 3
#define SIXTH_TOKEN 4
#define SEVENTH_TOKEN 5
#define EIGTH_TOKEN 6
#define NINTH_TOKEN 7
#define ROOT_CHAR 0
#define TOKEN_AMOUNT_PUT 8
#define TOKEN_AMOUNT_GET_HEAD_DELETE 4
#define SPACE_FOR_3_DELIMITERS 20
#define MYPROJECTKEY 73
#define CHARS_TILL_MSJ_BODY 4
#define SIZE_MENSAJE_CUERPO 1000
#define MAX_REQUEST_AUX 3000
#define MAX_LENGTH_DATA 5000




// Verificar formato HTTP y armar Estructura Request
int verificar_formato_HTTP (char request[], Request *req, int flag_sync);

// Chequear Header Content-Length completamente numerico
int all_numbers(char *Content_Length);

#endif
