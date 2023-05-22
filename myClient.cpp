#include<iostream>
#include<stdio.h>
#include <arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>

using namespace std; 

#define PORT 8080

int main()
{
    int status, valread, client_fd;

    struct sockaddr_in serv_addr;

    char* ack = (char*)"Data received";
    char buffer[1024] = { 0 };

    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;                                                             // for IPv4                                                                      
    serv_addr.sin_port = htons(PORT);                                                           // assigning server port number
    
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)                               // converts given string in to network address structure
                                                                                                // type ptovided by first argument (AF_INET --> IPv4)
                                                                                                // assign final output at destination pointed by 3rd argument
    {
        perror("Invalid address \n");
        exit(EXIT_FAILURE);
    }

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)     // Trying to connect with server 
    {
        perror("Connection Failed \n");
        exit(EXIT_FAILURE);
    }

    valread = read(client_fd, buffer, sizeof(buffer));
    if(valread < 0)
    {
        perror("error at reading");
        exit(EXIT_FAILURE);
    }
    
    cout << buffer << endl;

    write(client_fd, ack, strlen(ack));

    close(client_fd);                                                                           // closing the connected socket

    return 0;                                                                                       
}