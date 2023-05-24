#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

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

    if ((clientFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
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

   // Handling connect function in the case of NON_BLOCKING client
    if ((valid = connect(clientFd, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0)
    {
        if (errno != EINPROGRESS)
        {
            perror("Connection failed");
            exit(EXIT_FAILURE);
        }

        // Connection in progress, wait for it to complete or timeout
        fd_set writeSet;
        FD_ZERO(&writeSet);
        FD_SET(clientFd, &writeSet);

        struct timeval timeout;
        timeout.tv_sec = 5; // Set timeout to 5 seconds
        timeout.tv_usec = 0;

        int selectResult = select(clientFd + 1, NULL, &writeSet, NULL, &timeout);
        if (selectResult == -1)
        {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }
        else if (selectResult == 0)
        {
            perror("Connection timeout");
            exit(EXIT_FAILURE);
        }
        else
        {
            int error;
            socklen_t errorLen = sizeof(error);

            // Check if connection succeeded or failed
            if (getsockopt(clientFd, SOL_SOCKET, SO_ERROR, &error, &errorLen) == -1)
            {
                perror("Getsockopt failed");
                exit(EXIT_FAILURE);
            }

            if (error != 0)
            {
                fprintf(stderr, "Connection failed with error: %s\n", strerror(error));
                exit(EXIT_FAILURE);
            }
        }
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
        /**
         *  since client socket is also non-blocking
         *  sleep to ensure successfull server reply !  
        */
        // sleep(1);                       
        valid = recv(clientFd, buffer, sizeof(buffer), 0);
        buffer[valid] = 0;
        if (valid == 0)
        {
            printf("connection refused (received 0 Bytes)\n");
            close(clientFd);
            exit(EXIT_FAILURE);
        }
        else if(valid > 0)
        {
            printf("From server : %s\n\n", buffer);
        }
    }
    return 0;
}