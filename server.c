#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define DEFAULT_PORT 8080 // Definimos el puerto que usaremos
#define MAX_BUFFER_SIZE 20000 // definimos el buffer maximo para poder almacenar datos

/**
 * @brief Función que verifica si un string es un número.
 * @param number String a verificar.
 * @return int 1 si es un número, 0 si no lo es.
 */
int isNumber(char* number)
{
    int i = 0;
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++) {
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
            fprintf(stderr, "Servidor SSH Demo -- Proyecto Final Arquitectura Cliente-Servidor\n\nComandos:\n%s: Inicializa el servidor con el puerto default 8080 en localhost (127.0.0.1).\n%s <NUMERO_PUERTO>: Inicializa el servidor en localhost (127.0.0.1) utilizando el puerto ingresado.\n%s -h: Muestra mensaje de ayuda.\n\n", argv[0], argv[0], argv[0]);
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

    // Se crean los file descriptors para el servidor y el socket
    int server_fd, socket_fd;

    // Se crea la estructura para la dirección del servidor
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configuramos la dirección del servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server_port);

    // Enlazamos el socket con la dirección del servidor
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Error en el enlace");
        exit(EXIT_FAILURE);
    }

    // Escuchamos en el socket del servidor
    if (listen(server_fd, 3) < 0) {
        perror("Error al escuchar");
        exit(EXIT_FAILURE);
    }

    printf("Servidor en IP 127.0.0.1 (localhost) escuchando en el puerto %d\n", server_port);

    // Aceptamos la conexión entrante
    if ((socket_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Error al aceptar la conexión");
        exit(EXIT_FAILURE);
    }

    // Declaración de Buffers para almacenar los datos recibidos y la respuesta
    char buffer[MAX_BUFFER_SIZE];
    char response_buffer[MAX_BUFFER_SIZE];
    int valread;

    while (1) {
        // Limpiamos el buffer y recibimos los datos
        bzero(buffer, MAX_BUFFER_SIZE);
        valread = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
        if (valread == -1) {
            perror("recv");
            break;
        }
        buffer[valread] = '\0';

        // Se separa el usuario del comando para ejecutarlo
        char user[50], command[MAX_BUFFER_SIZE];
        if (sscanf(buffer, "%49[^:]: %[^\n]", user, command) != 2) {
            printf("Cadena recibida: %s", buffer);
            continue;
        }

        // Si el comando es "exit" se cierra la conexión
        if (strcmp(command, "exit") == 0) {
            printf("Usuario %s salió\n", user);
            break;
        }

        printf("Usuario: %s, Comando: %s\n", user, command);

        // Se crea un pipe para redirigir la salida del comando a la respuesta
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            continue;
        }

        // Se crea un proceso hijo para ejecutar el comando
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            continue;
        } else if (pid == 0) { // Proceso hijo
            close(pipefd[0]); // Cerramos el lado de lectura del pipe
            dup2(pipefd[1], STDOUT_FILENO); // Redirigimos stdout al pipe
            dup2(pipefd[1], STDERR_FILENO); // Redirigimos stderr al pipe
            close(pipefd[1]); // Cerramos el lado de escritura del pipe

            // Ejecutamos el comando con execlp
            execlp("sh", "sh", "-c", command, NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        } else { // Proceso padre
            close(pipefd[1]); // Cerramos el lado de escritura del pipe

            bzero(response_buffer, MAX_BUFFER_SIZE);
            int nbytes;

            // Leemos la respuesta del comando y la enviamos al cliente
            // Si no hay respuesta, enviamos un espacio
            if ((nbytes = read(pipefd[0], response_buffer, MAX_BUFFER_SIZE - 1)) <= 0) {
                if (send(socket_fd, " ", 1, 0) == -1) {
                    perror("send");
                    break;
                }
            } else {
                do {
                    response_buffer[nbytes] = '\0';
                    if (send(socket_fd, response_buffer, nbytes, 0) == -1) {
                        perror("send");
                        break;
                    }
                    bzero(response_buffer, MAX_BUFFER_SIZE);
                } while ((nbytes = read(pipefd[0], response_buffer, MAX_BUFFER_SIZE - 1)) > 0);
            }

            close(pipefd[0]); // Cerramos el lado de lectura del pipe

            waitpid(pid, NULL, 0); // Esperamos a que el proceso hijo termine
        }
    }

    close(socket_fd);
    close(server_fd);
    return 0;
}
