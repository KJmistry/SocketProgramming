#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 8080
#define EXIT_FAILURE 1
#define RCV_BUFFER_SIZE 20
#define MAX_CONNECTION 2

int main()
{

    int serverFd, newSocket = -1, valid, opt = 1, tIndex = 0, totalConnection = 0;
    int sockFdArr[MAX_CONNECTION];

    struct sockaddr_in server_address, clientAddress;
    int clientAddrlen = sizeof(clientAddress);

    char receivedString[RCV_BUFFER_SIZE] = {0};

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

    if (listen(serverFd, 3) < 0)
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
        // if (newSocket < 0)
        // {
        //     perror("error at accept");
        //     sleep(1);
        // }
        // else
        if (newSocket > 0)
        {
            printf("connection request !\n");
            if (totalConnection == MAX_CONNECTION)
            {
                close(newSocket);
            }
            for (tIndex = 0; tIndex < MAX_CONNECTION; tIndex++)
            {
                if (sockFdArr[tIndex] < 0)
                {
                    sockFdArr[tIndex] = newSocket;
                    totalConnection++;
                    break;
                }
            }
            printf("Total active connections : %d\n\n", totalConnection);

        }

        for (tIndex = 0; tIndex < MAX_CONNECTION; tIndex++)
        {
            if (sockFdArr[tIndex] > 0)
            {
                valid = recv(sockFdArr[tIndex], receivedString, sizeof(receivedString), MSG_DONTWAIT);
                // if (valid < 0)
                // {
                //     perror("error at reading");
                // }
                if (valid == 0)
                {
                    printf("Received 0 Byte !, server disconnected !\n");
                    close(sockFdArr[tIndex]);
                    sockFdArr[tIndex] = -1;
                    totalConnection--;
                    printf("Total active connections : %d\n\n", totalConnection);
                }

                else if (valid > 0)
                {
                    receivedString[valid] = 0;
                    printf("Received string from client fd %d : %s\n\n", sockFdArr[tIndex], receivedString);
                    valid = send(sockFdArr[tIndex], "Message Received", 16, MSG_NOSIGNAL);
                    if (valid <= 0)
                    {
                        perror("error while sending");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }
    return 0;
}