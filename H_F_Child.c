#include "H_F_Child.h"


// Verificar formato HTTP y armar Estructura Request
int verificar_formato_HTTP (char request[], Request *req, int flag_sync){
		
	// Spliteo de tokens del request		
	char *saveptr;
	char *token;
	char delimiters[SPACE_FOR_3_DELIMITERS] = "\r\n "; // CRLF y space
	int parse = SECOND_TOKEN;
	int formato_check = FORMAT_OK;
	int token_correct_amount = TOKEN_AMOUNT_GET_HEAD_DELETE;
	char message_body[SIZE_MENSAJE_CUERPO];
	
	token = strtok_r(request, " ", &saveptr);
	printf("\nParseo:\ntoken: %s\n",token);
	
	// Revisar metodo HTTP
	if ((flag_sync > 0) && (!(strcmp(token, "GET") == 0 || strcmp(token, "SYNCP") == 0 || strcmp(token, "HEAD") == 0 || strcmp(token, "PUT") == 0
				|| strcmp(token, "DELETE") == 0 || strcmp(token, "SYNCD") == 0 )) ){
		formato_check = NOT_IMPLEMENTED;
	} else if ( (flag_sync == 0) &&  (!(strcmp(token, "GET") == 0 || strcmp(token, "HEAD") == 0 || strcmp(token, "PUT") == 0 || strcmp(token, "DELETE") == 0 )) ) {
		formato_check = NOT_IMPLEMENTED;
	}
	else{
		if(strcmp(token,"PUT") == 0 || strcmp(token, "SYNCP") == 0){
			token_correct_amount = TOKEN_AMOUNT_PUT;
		}
		else{
			strcpy(req->content_length, "empty");
			strcpy(req->content_type, "empty");
			strcpy(req->message_body, "empty");
		}	
		strcpy(req->method,token);			
	}	
	while((token = strtok_r(NULL, delimiters, &saveptr)) != NULL && formato_check == FORMAT_OK && parse != token_correct_amount){
		printf("token: %s\n", token);
		if(parse == SECOND_TOKEN){
			// Revisar URL
			if(token[ROOT_CHAR] !='/'){
				formato_check = BAD_REQUEST;
				printf("Falta root\n");	
			}
			else{
				strcpy(req->url,token);
			}
		}
		if(parse == THIRD_TOKEN){
			// Revisar version HTTP 
			if(strcmp(token, "HTTP/1.0") == 0 ){
				formato_check = NOT_SUPPORTED;	
			}
			else{
				if(strcmp(token, "HTTP/1.1") != 0 ){
					formato_check = BAD_REQUEST;
					printf("Version mal escrita\n");	
				}
			}		
		}
		if(parse == FOURTH_TOKEN){
			// Revisar HOST
			 if(strcmp(token, "Host:") != 0 && strstr(token, "\\nHost") == NULL){
			 	formato_check = BAD_REQUEST;
				printf("Host escrito mal\n");	
			 }
		}
		if(parse == FIFTH_TOKEN){
			// Revisar Server IP
			 if(strcmp(token,"localhost:8080") != 0){ //-> Modificar aqui IP del servidor
				printf("localhost 8080 mal escrito\n"); 
			 	formato_check = BAD_REQUEST;
			 }
			 else{
				strcpy(req->host, token);
				
			 }
		}
		if(token_correct_amount == TOKEN_AMOUNT_PUT){
			if(parse == SIXTH_TOKEN){
				//Revisar Content-Length
				if(strstr(token,"\\nContent-Length:") == NULL){
					printf("Content-Length: mal escrito\n");
				 	formato_check = BAD_REQUEST;
				}
			}
			if(parse == SEVENTH_TOKEN){
				//Revisar si es numero
				if(all_numbers(token) == BAD_REQUEST ){ 
					printf("Content-Length no es un numero\n");
				 	formato_check = BAD_REQUEST;
				}
				else{
					strcpy(req->content_length, token);
			 	}
			}
			if(parse == EIGTH_TOKEN){
				//Revisar Content-Type
				if(strstr(token,"\\nContent-Type:") == NULL){ 
					printf("Content-Type: mal escrito\n");
				 	formato_check = BAD_REQUEST;
				}
			}
			if(parse == NINTH_TOKEN){
				//Revisar tipo adecuado
				if(strcmp(token,"text/plain") != 0 && strcmp(token,"text/html") != 0 ){
					printf("no es text/plain o text/html \n");
				 	formato_check = BAD_REQUEST;
				}
				else{				
					strcpy(req->content_type, token);
					token = strtok_r(NULL, " ", &saveptr);
					if((strstr(token, "\\n\\n")) == NULL){
						formato_check = BAD_REQUEST;
						printf("falta salto de linea\n");
					}
					token = token + CHARS_TILL_MSJ_BODY; //pointer al message body
					strcat(message_body, token);
					while((token = strtok_r(NULL, " ", &saveptr)) != NULL){
						strcat(message_body, " ");
						strcat(message_body, token);
					}
					printf("message_body:%s\n", message_body);
					strcpy(req->message_body, message_body);
				}
			}
		}
		parse++;
	}
	if(parse < token_correct_amount && formato_check == FORMAT_OK ){
		formato_check = BAD_REQUEST;
		printf("Cantidad de tokens incorrecta \n");
	}
	strcpy(req->request_status, "queueing");
	return formato_check;
}


// Chequear Header Content-Length completamente numerico
int all_numbers(char *Content_Length){
	int i;
	int all_numbers = EXIT_SUCCESS;
	for(i = 0; i<strlen(Content_Length); i++){
		if(isalpha(Content_Length[i])){
			all_numbers = BAD_REQUEST;
			printf("char:%c",Content_Length[i]);
			i = strlen(Content_Length);
			
		}
	}
	return all_numbers;
}

