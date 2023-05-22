#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define EXIT_FAILURE 1

// void sigpipeHandler(int s) 
// {
//     printf("Connection is broken\n");
//     exit(0);
// }

int main(void)
{
    int status, clientFd, valid, opt = 1;

    struct sockaddr_in servAddr;
    // signal(SIGPIPE, sigpipeHandler);

    char buffer[100] = {0};
    char finalData[100];

    if ((clientFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);


    if (inet_pton(AF_INET, SERVER_IP, &servAddr.sin_addr) <= 0)
    {
        printf("Invalid address/ Address not supported : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if ((status = connect(clientFd, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0)
    {
        printf("Connection Failed : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        scanf("%s",finalData);
        valid = send(clientFd, finalData, strlen(finalData), MSG_NOSIGNAL);
        if (valid <= 0)
        {
            perror("send fail");
            break;
        }

        valid = recv(clientFd, buffer, sizeof(buffer), 0);
        if (valid <= 0)
        {
            printf("receive Failed !\nerrno : %d %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("From server : %s\n\n", buffer);
    }

    return 0;
}