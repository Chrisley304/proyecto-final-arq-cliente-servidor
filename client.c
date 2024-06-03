#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

int main(int argc, char const* argv[])
{

    // Verificamos que el comando de terminal tenga los 3 argumentos necesarios
    //./ejecutable IP Puerto
    // Si no se detectan los 3 lo que haremos sera enviar un error
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP DEL SERVIDOR> <PUERTO>\n", argv[0]);
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

    // Configuramos nuestra estrcutura para la info del servidor
    servidor_address.sin_family = AF_INET;
    servidor_address.sin_port = htons(atoi(argv[2]));

    // Convertimos la dirección IP a binario para la conexion
    // En caso de error mandamos mensaje
    if (inet_pton(AF_INET, argv[1], &servidor_address.sin_addr) <= 0) {
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
    char buffer[MAX_BUFFER_SIZE] = { 0 };
    printf("Ingrese el comando a enviar al servidor: ");
    // Con fgets vamos a leer una linea de texto desde el teclado
    // Los parametros son: donde se almacena, tamaño maximo, indicamos que es de teclado
    fgets(buffer, sizeof(buffer), stdin);

    // Una vez leido el comando lo Enviamos al servidor
    // En este caso usamos se usa el file descriptor del cliente, el comando, longitud de los datos a enviar
    // El 0 es para indicar que no hay banderas adicionales
    send(file_descriptor_servidor, buffer, strlen(buffer), 0);

    // Recibimos la salida del servidor
    printf("Respuesta del servidor:\n");

    // Se ejecutara hasta que lleguemos a un break
    while (1) {
        // Recibiremos datos del servidor
        // parametros fileDescriptor, lugar donde se alamcena, tamaño maximo de datos a recibir
        ssize_t mensaje_recibido = recv(file_descriptor_servidor, buffer, sizeof(buffer), 0);
        if (mensaje_recibido <= 0) {
            break;
        }
        printf("%.*s", (int)mensaje_recibido, buffer);
    }

    // Cerrar el socket y terminar el programa
    close(file_descriptor_servidor);
    return 0;
}
