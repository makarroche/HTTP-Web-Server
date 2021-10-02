#Servidor HTTP 1.1
#Compilacion

Para compilar ejecutar el archivo compilation.sh

#Ejecucion 
1- Este sistema se compone de un bloque Backends que debe ser ejecutado en consolas distintas o en maquinas distintas.
Para dicha ejecucion se debe utilizar el archivo inicioBE.sh

2- Ejecutar el Servidor HTTP 1.1 Sistema general
Para la ejecucion ejecutar el archivo inicioSistema.sh
Verificar quese creen los 3 cadetes puerto 6000. 6001 . 6002 y que HTTP management y Handler Front end inicien de forma normal en escucha

3- Para probar el sistema se compone de 2 partes:

	a - Se puede probar el request ejecutando el archivo  inicioCliente.sh o un request desde un navegador
	b - Para probar las consultas del sistema ejecutar el archivo inicioConsola.sh


En su defecto ejecutar todos los archivos en el siguiente orden en distintas terminales
./Backend_Personal_ Manager 7000
./Backend_Personal_ Manager 7001
./Backend_Personal_ Manager 7002
./Handler_Backend
./HTTP_Management
./Handler_Frontend_2
./Console_Manager

Finalmente

./Cliente


//Para pruebas: CORRECTAS *****************************************************************************************

//GET /index.html HTTP/1.1 \nHost: localhost:8080
//HEAD /index.html HTTP/1.1 \nHost: localhost:8080
//DELETE /index.html HTTP/1.1 \nHost: localhost:8080
//PUT /index.html HTTP/1.1 \nHost: localhost:8080 \nContent-Length: 12 \nContent-Type: text/plain \n\nHello world! 
// PUT /archivo.txt HTTP/1.1 \nHost: localhost:8080 \nContent-Length: 12 \nContent-Type: text/plain \n\nHello world!

//Para pruebas: ERRORES EN FORMATO *********************************************************************************

//GET /index.html 
//TRACE /index.html HTTP/1.1 \nHost: localhost:8080
//get /index.html HTTP/1.1 \nHost: localhost:8080
//GET index.html HTTP/1.1 \nHost: localhost:8080
//GET /index.html HTTP/1.0 \nHost: localhost:8080

Para consulta de datos ejecutar:

./consola
