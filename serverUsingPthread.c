#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#define PORT 8080
#define EXIT_FAILURE 1
#define RCV_BUFFER_SIZE 20
#define MAX_CONNECTION 2

void *manageConnection(void *);

int clientStatusArray[MAX_CONNECTION];

typedef struct
{
    int clientFd;
    int clientIndex;
} clientInfo;

int main()
{

    pthread_t clientThreadArray[MAX_CONNECTION];
    pthread_attr_t attr;
    // Initialize the thread attributes as detached
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    memset(&clientStatusArray, 0, sizeof(int) * MAX_CONNECTION);

    clientInfo clientInfoArray[MAX_CONNECTION];

    int serverFd, newSocket = -1, opt = 1, tIndex = 0, totalConnection = 0;
    int sockFdArr[MAX_CONNECTION];

    struct sockaddr_in server_address, clientAddress;
    int clientAddrlen = sizeof(clientAddress);

    if ((serverFd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(serverFd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverFd, MAX_CONNECTION) < 0)
    {
        perror("error at listen");
        exit(EXIT_FAILURE);
    }

    for (tIndex = 0; tIndex < MAX_CONNECTION; tIndex++)
    {
        sockFdArr[tIndex] = -1;
    }

    while (1)
    {
        newSocket = accept(serverFd, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddrlen);
        if ((newSocket < 0) && (errno != EAGAIN))
        {
            perror("error at accept");
        }

        if (newSocket > 0)
        {
            printf("connection request !\n");

            for (tIndex = 0; tIndex < MAX_CONNECTION; tIndex++)
            {
                if (sockFdArr[tIndex] < 0)
                {
                    sockFdArr[tIndex] = newSocket;

                    // start thread
                    clientInfoArray[tIndex].clientIndex = tIndex;
                    clientInfoArray[tIndex].clientFd = newSocket;

                    if (pthread_create(&clientThreadArray[tIndex], &attr, &manageConnection, &clientInfoArray[tIndex]) != 0)
                    {
                        perror("Failed creating thread");
                        close(newSocket);
                        continue;
                    }

                    totalConnection++;
                    break;
                }
                if (tIndex == MAX_CONNECTION - 1)
                    close(newSocket);
            }
            printf("Total active connections : %d\n\n", totalConnection);
        }

        for (tIndex = 0; tIndex < MAX_CONNECTION; tIndex++)
        {
            if (clientStatusArray[tIndex] < 0)
            {
                sockFdArr[tIndex] = -1;
                totalConnection--;
                clientStatusArray[tIndex] = 0;
                printf("Total active connections : %d\n\n", totalConnection);
            }
        }
    }
    return 0;
}

void *manageConnection(void *args)
{
    printf("thread started!\n");
    char receivedString[RCV_BUFFER_SIZE] = {0};
    int valid;
    clientInfo *connectionInfo = (clientInfo *)args;
    while (1)
    {
        valid = recv(connectionInfo->clientFd, receivedString, sizeof(receivedString), MSG_DONTWAIT);
        if ((valid < 0) && (errno != EAGAIN))
        {
            perror("error at reading");
        }

        if (valid == 0)
        {
            printf("Received 0 Byte !, client disconnected !\n");
            close(connectionInfo->clientFd);
            clientStatusArray[connectionInfo->clientIndex] = -1;
            pthread_exit(NULL);
        }

        else if (valid > 0)
        {
            receivedString[valid] = 0; // ensuring NULL termination

            printf("Received string from client fd %d : %s\n\n", connectionInfo->clientFd, receivedString);

            valid = send(connectionInfo->clientFd, "Message Received", 16, MSG_NOSIGNAL);
            if (valid <= 0)
            {
                perror("error while sending");
                close(connectionInfo->clientFd);
                clientStatusArray[connectionInfo->clientIndex] = -1;
                pthread_exit(NULL);
            }
        }
    }
}