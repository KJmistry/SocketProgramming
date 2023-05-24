#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define EXIT_FAILURE 1

#define RCV_BUFFER_SIZE 20

int main(void)
{
    int clientFd, valid;

    struct sockaddr_in servAddr;

    char buffer[RCV_BUFFER_SIZE] = {0};
    char sendBuffer[30];

    if ((clientFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &servAddr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    if ((valid = connect(clientFd, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0)
    {
        perror("Connection Failed \n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        scanf("%s", sendBuffer);

        valid = send(clientFd, sendBuffer, strlen(sendBuffer), MSG_NOSIGNAL);
        if (valid < 0)
        {
            perror("error while sending");
            close(clientFd);
            exit(EXIT_FAILURE);
        }

        valid = recv(clientFd, buffer, sizeof(buffer), 0);
        buffer[valid] = 0;
        if (valid <= 0)
        {
            printf("connection refused (received 0 Bytes)\n");
            close(clientFd);
            exit(EXIT_FAILURE);
        }
        printf("From server : %s\n\n", buffer);
    }
    return 0;
}