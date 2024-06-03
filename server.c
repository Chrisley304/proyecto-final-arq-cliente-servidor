#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_PORT 8080 // Definimos el puerto que usaremos
#define MAX_BUFFER_SIZE 1024 // definimos el buffer maximo para poder almacenar datos

/**
 * @brief Función que verifica si un string es un número.
 * @param number String a verificar.
 * @return int 1 si es un número, 0 si no lo es.
 */
int isNumber(char* number)
{
    int i = 0;

    // checking for negative numbers
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++) {
        // if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return 0;
    }
    return 1;
}

/**
 * @brief Función principal del servidor SSH. Por defecto escucha en el puerto port 8080 en localhost (127.0.0.1).
 * @param argc Número de argumentos pasados al programa.
 * @param argv Argumentos pasados al programa. `argv[1]`(opcional) es el puerto en el que escuchará el servidor.
 *
 * @return int
 */
int main(int argc, char* argv[])
{
    int server_port = DEFAULT_PORT;
    // Verificamos los argumentos pasados al programa, si hay alguno.
    if (argc >= 2) {
        // -h -> ayuda
        if (strcmp(argv[1], "-h") == 0) {
            fprintf(stderr, "Servidor SSH Demo -- Proyecto Final Arquitectura Cliente-Servidor\n\nComandos:\n- ./server -h: Muestra mensaje de ayuda.\n./server: Inicializa el servidor con el puerto default 8080 en localhost (127.0.0.1).\n./server <NUMERO_PUERTO>: Inicializa el servidor en localhost (127.0.0.1) utilizando el puerto ingresado.\n\n");
            exit(EXIT_SUCCESS);
        } else {
            // [int] -> puerto
            if (isNumber(argv[1])) {
                server_port = atoi(argv[1]);
            } else {
                fprintf(stderr, "Error: El puerto '%s' es incorrecto. El puerto debe ser un número entero.\n", argv[1]);
                exit(EXIT_FAILURE);
            }
        }
    }

    int server_fd, new_socket; // File Descriptor og servidor
    struct sockaddr_in address;
    int addrlen = sizeof(address); // tamaño de direccion

    // Creación del socket para el servidor.
    // Sí la respuesta es diferente de 0, un error es enviado al conectar.
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; // La direccion del servidor sera de tipo AF_INET
    address.sin_addr.s_addr = INADDR_ANY; // Direccion ip que utilizaremos (localhost -> 127.0.0.1)

    address.sin_port = htons(server_port); // Puerto que el servidor escuchara

    // Enlazando nuestro socket a la dirección y el puerto indicado
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Error en el enlace");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones
    // Utilizamos la funcion listen para escuchar las conexiones
    // listen recibe el file descriptor del servidor, el numero de conexiones
    // Verificamos si la respuesta regresa -1 para validar si hay errores
    // En este caso podemos manejar hasta 3 conexiones entrantes
    if (listen(server_fd, 3) < 0) {
        perror("Error al escuchar");
        exit(EXIT_FAILURE);
    }

    printf("Servidor en IP 127.0.0.1 (localhost) escuchando en el puerto %d\n", server_port);

    // Aceptamos la conexión entrante
    // Una vez aceptada la conexion creamos un nuevo socket para la comunicacion con el cliente
    // En caso de que exista un error mandamos el mensaje
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Error al aceptar la conexión");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_BUFFER_SIZE] = { 0 };

    // Leemos el comando enviado por el cliente y lo almacenamos en el buffer
    // Posterior imprimimos el comando recibido
    read(new_socket, buffer, sizeof(buffer));
    printf("Comando recibido: %s\n", buffer);

    // Ejecutamos el comando y obtenemos salida
    // con el comando popen se abre un proceso que ejecuta un comando como si estuvieramos en terminal
    // El primer argumento de popen es el comando en este caso buffer
    // El segundo argumento es como abrimos el archivo en este caso para lectura
    // En caso de error mandamos el error
    FILE* fp = popen(buffer, "r");
    if (fp == NULL) {
        perror("Error al ejecutar el comando");
        exit(EXIT_FAILURE);
    }

    // Leemos la salida del comando y la enviamos al cliente
    // mientras el buffer tenga caracteres seguimos enviando la informacion al cliente
    while (fgets(buffer, sizeof(buffer) - 1, fp) != NULL) {
        send(new_socket, buffer, strlen(buffer), 0);
    }

    // Cerrar el socket y terminar el programa
    close(new_socket);
    return 0;
}
