#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8080
#define EXIT_FAILURE 1
#define MAX_CONNECTION_IN_QUEUE 5
#define MAX_CONNECTIONS 2
#define MAX_RECV_SIZE 30
#define MASTER_FD_INDEX 0

int acceptConection(int, fd_set, int *, int *, int *);
int manageExistingConnections(int*, int *, fd_set);

int main()
{
    int masterFd, validation, maxFd, opt = 1, index = 0, totalFd = 0;
    fd_set readFdSet;
    struct sockaddr_in serverAddress;
    int fdArray[MAX_CONNECTIONS + 1];

    if ((masterFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    serverAddress.sin_family = AF_INET;         
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
    serverAddress.sin_port = htons(PORT);       

    if (setsockopt(masterFd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(masterFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(masterFd, MAX_CONNECTION_IN_QUEUE) < 0)
    {
        perror("error at listen");
        exit(EXIT_FAILURE);
    }

    for (index = 0; index < MAX_CONNECTIONS + 1; index++)
    {
        fdArray[index] = -1;
    }

    fdArray[MASTER_FD_INDEX] = masterFd;
    maxFd = masterFd;
    totalFd++;

    while (1)
    {
        printf("Total connections : %d \n", totalFd - 1);

        FD_ZERO(&readFdSet);

        for (index = 0; index < MAX_CONNECTIONS + 1; index++)
        {
            printf("fdList [%d] = %d \n", index, fdArray[index]);

            if (fdArray[index] >= 0)
                FD_SET(fdArray[index], &readFdSet);
        }

        if (select(maxFd + 1, &readFdSet, NULL, NULL, NULL) <= 0)
        {
            printf("error in select\n");
        }

        if (FD_ISSET(fdArray[MASTER_FD_INDEX], &readFdSet))
        {
            printf("connection req from client\n");
            acceptConection(masterFd, readFdSet, fdArray, &totalFd, &maxFd);
        }
        
        validation = manageExistingConnections(fdArray, &totalFd, readFdSet);

        if (validation < 0)
        {
            printf("Error occured");
        }

    }
    return 0;
}

int acceptConection(int masterFd, fd_set readFdSet, int *fdArray, int *totalFdPtr, int *maxFdPtr)
{   
    struct sockaddr_in clientAddress;
    int newSockFd, index = 0, clientAddrlen = sizeof(clientAddress);
    
    if ((newSockFd = accept(masterFd, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddrlen)) < 0)
    {
        perror("error at accept");
        return 1;
    }

    if((*totalFdPtr) == MAX_CONNECTIONS + 1)
    {   
        close(newSockFd);
        return 0;
    }

    for (index = 0; index < MAX_CONNECTIONS + 1; index++)
    {
        if (fdArray[index] < 0)
        {
            fdArray[index] = newSockFd;
            *totalFdPtr += 1;
            break;
        }
    }

    if (newSockFd > *(maxFdPtr))
    {
        *(maxFdPtr) = newSockFd;
    }
    return 0;
}


int manageExistingConnections(int* fdArray, int *totalFdPtr, fd_set readFdSet)
{   
    int validation, index;
    char receivedString[MAX_RECV_SIZE];
    
    for (index = 1; index < MAX_CONNECTIONS + 1; index++)
    {   
        if (FD_ISSET(fdArray[index], &readFdSet))
        {
            validation = recv(fdArray[index], receivedString, sizeof(receivedString), 0);
            if (validation <= 0)
            {
                printf("Recv failed!, client not connected !\n");
                close(fdArray[index]);
                fdArray[index] = -1;
                (*totalFdPtr) -= 1;
                continue;
            }
            printf("Received string from client fd = %d : %s\n", fdArray[index], receivedString);
            memset(receivedString, 0, sizeof(receivedString));

            // validation = send(fdArray[index], "Received !", 10, 0);
            // if (validation <= 0)
            // {
            //     printf("send failed !\n");
            //     close(fdArray[index]);
            //     fdArray[index] = -1;
            //     (*totalFdPtr) -= 1;
            // }
        }
    }
    return 0;    
}

