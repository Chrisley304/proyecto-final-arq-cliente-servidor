#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char* command = "ls -la";
    char buffer[1024] = { 0 };

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        return -1;
    }

    if (send(sock, command, strlen(command), 0) < 0) {
        printf("\nSending command failed\n");
        return -1;
    }
    printf("Command sent\n");

    // Clear buffer
    memset(buffer, 0, sizeof(buffer));

    int valread;
    if ((valread = read(sock, buffer, 1024)) < 0) {
        printf("\nReading response failed\n");
        return -1;
    }
    printf("%s\n", buffer);

    // Close the socket
    close(sock);

    return 0;
}