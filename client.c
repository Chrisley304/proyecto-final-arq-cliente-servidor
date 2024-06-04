#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 20000

/**
 * @brief Función principal del cliente SSH.
 *
 * @param argc Número de argumentos pasados al programa.
 * @param argv Argumentos pasados al programa. `argv[1]` es el usuario, IP y puerto del servidor SSH.
 * @return int
 */
int main(int argc, char const* argv[])
{

    // Verificamos que el comando de terminal tenga los argumentos necesarios o si se solicita ayuda mediante -h
    if (argc >= 2 && argv[1][0] == '-' && argv[1][1] == 'h') {
        printf("Cliente SSH Demo -- Proyecto Final Arquitectura Cliente-Servidor\n\nUso:\n%s <USUARIO>@<IP_DEL_SERVIDOR>:<PUERTO>\n\nDONDE: <USUARIO> es el nombre de usuario del servidor SSH.\n<IP_DEL_SERVIDOR> es la dirección IP del servidor SSH.\n<PUERTO> es el puerto en el que escucha el servidor SSH.\nPara salir del prompt una vez conectado ingrese 'exit'.\n\n", argv[0]);
        exit(EXIT_SUCCESS);
    } else if (argc < 2) {
        printf("ERROR: Parametro faltante. Utiliza -h para ver información de uso.\n");
        exit(EXIT_FAILURE);
    }

    char client_user[50], server_ip[50];
    int server_port;

    // Se parsea el formato <usuario>@<servidor>:<puerto>
    if (sscanf(argv[1], "%49[^@]@%49[^:]:%d", client_user, server_ip, &server_port) != 3) {
        fprintf(stderr, "Formato incorrecto. Uso: %s <usuario>@<IP_DEL_SERVIDOR>:<PUERTO>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Declaramos variable para el file descriptor del cliente
    int file_descriptor_servidor = 0;

    // Declaramos la estructura para la informacion del servidor
    struct sockaddr_in servidor_address;

    // Creamos el socket del cliente con la funcion socket
    // En caso de que exista un error lo mandamos
    if ((file_descriptor_servidor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // printf("Conectando al usuario %s al servidor %s:%d\n", client_user, server_ip, server_port);

    // Configuramos nuestra estrcutura para la info del servidor
    servidor_address.sin_family = AF_INET;
    servidor_address.sin_port = htons(server_port);

    // Convertimos la dirección IP a binario para la conexion
    // En caso de error mandamos mensaje
    if (inet_pton(AF_INET, server_ip, &servidor_address.sin_addr) <= 0) {
        perror("Dirección IP no válida");
        exit(EXIT_FAILURE);
    }

    // Conectando con el servidor
    // Utilizamos la funcion conect para establecer conexion con el socket del cliente
    // En caso contrario mandamos error
    if (connect(file_descriptor_servidor, (struct sockaddr*)&servidor_address, sizeof(servidor_address)) < 0) {
        perror("Error al conectar");
        exit(EXIT_FAILURE);
    }

    // Declaramos nuestra variable para almacenar el comando que venga de consola
    char buffer[MAX_BUFFER_SIZE];
    char response_buffer[MAX_BUFFER_SIZE];
    int mensaje_recibido;
    while (1) {
        // Imprimimos el prompt del cliente
        printf("%s@%s:%d> ", client_user, server_ip, server_port);
        fgets(buffer, sizeof(buffer), stdin);

        // Elina el caracter de nueva linea
        buffer[strcspn(buffer, "\n")] = 0;

        // Terminar el programa si el comando es "exit"
        if (strcmp(buffer, "exit") == 0) {
            snprintf(buffer, sizeof(buffer), "%s: exit", client_user);
            send(file_descriptor_servidor, buffer, strlen(buffer), 0);
            break;
        }

        // Enviar el nombre de usuario y el comando al servidor
        char message[MAX_BUFFER_SIZE] = { 0 };
        snprintf(message, sizeof(message), "%s: %s", client_user, buffer);

        // Enviamos el comando al servidor
        if (send(file_descriptor_servidor, message, strlen(message), 0) == -1) {
            perror("send");
            break;
        }

        // Recibimos la respuesta del servidor
        // Recibiremos datos del servidor
        mensaje_recibido = recv(file_descriptor_servidor, response_buffer, sizeof(response_buffer) - 1, 0);
        if (mensaje_recibido <= 0) {
            perror("recv");
            break;
        }
        response_buffer[mensaje_recibido] = '\0';
        printf("%s", response_buffer);
        printf("\n");
    }

    // Cerrar el socket y terminar el programa
    close(file_descriptor_servidor);
    return 0;
}
