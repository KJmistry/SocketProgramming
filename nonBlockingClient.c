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
    volatile int checkConnection;
    struct sockaddr_in servAddr;

    char buffer[RCV_BUFFER_SIZE] = {0};

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
    if ((checkConnection = connect(clientFd, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0)
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

    valid = send(clientFd, "Hello from client", 17, MSG_NOSIGNAL);

    if (valid < 0)
    {
        perror("error while sending");
        exit(EXIT_FAILURE);
    }

    /*
      *server is not responding immediately
      *and also the socket is NON BLOCKING
      *hence need to check untill recv() reades data once
      *(as per current application !)
    */
    do          
    {
        valid = recv(clientFd, buffer, sizeof(buffer), 0);

        if (valid <= 0)
        {
            perror("error at reading");
            sleep(1);
        }
    } while (valid <= 0);                       


    printf("From server : %s\n\n", buffer);

    return 0;
}