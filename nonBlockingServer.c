#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 8080
#define EXIT_FAILURE 1
#define RCV_BUFFER_SIZE 20

int main()
{

    int serverFd, newSocket = -1, valid, opt = 1;

    struct sockaddr_in server_address,clientAddress;
    int clientAddrlen = sizeof(clientAddress);
        
    char receivedString[RCV_BUFFER_SIZE] = { 0 };
    
    if ((serverFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)                                            
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

    if (bind(serverFd, (struct sockaddr*)&server_address,sizeof(server_address)) < 0)                               
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(serverFd, 3) < 0)                                                                                    
    {
        perror("error at listen");
        exit(EXIT_FAILURE);
    }

    while (newSocket < 0)
    {
        if ((newSocket = accept(serverFd, (struct sockaddr*)&clientAddress, (socklen_t*)&clientAddrlen)) < 0)        
        {
            perror("error at accept");
            sleep(1);
        }
    }

    while (valid <= 0)
    {        
        valid = recv(newSocket, receivedString, sizeof(receivedString), 0);                                        
        
        if (valid <= 0)
        {
            perror("error at reading");
            sleep(1);
        }
    }

    printf("Received string from client : %s\n\n",receivedString);

    valid = send(newSocket,"Hello from server", 17, MSG_NOSIGNAL);

    if (valid <= 0)
    {
        perror("error while sending");
        exit(EXIT_FAILURE);
    }

    return 0;
}    