#include<iostream>
#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>

using namespace std; 

#define PORT 8080

int main()
{
    int server_fd, new_socket, valread;

    struct sockaddr_in server_address,client_address;
    int client_addrlen = sizeof(client_address);
        
    char buffer[1024] = { 0 };
    string iStrinng;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)                                                       // Creating socket file descriptor [IPv4 , TCP , protocol used (0 for IP)]
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;                                                                        // for IPv4                                                   
    server_address.sin_addr.s_addr = INADDR_ANY;                                                                // Accept connection form any Host
    server_address.sin_port = htons(PORT);                                                                      // Converts a port number in host byte order to a port number in network byte order 
                                                                                                                // ex: 0x1389 will be stored as --> 0x89 0x13

    if(bind(server_fd, (struct sockaddr*)&server_address,sizeof(server_address)) < 0)                           // Attaching socket to the port 8080
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 3) < 0)                                                                                // Listeinig for connection request from client (client limit = 3)
    {
        perror("error at listen");
        exit(EXIT_FAILURE);
    }

    if((new_socket = accept(server_fd, (struct sockaddr*)&client_address, (socklen_t*)&client_addrlen)) < 0)    // Accepting connection request and assigning new socket fd for communication
    {
        perror("error at accept");
        exit(EXIT_FAILURE);
    }

    getline(cin,iStrinng);

    write(new_socket, iStrinng.c_str(), iStrinng.size());                                                       // writing data to client 
    
    valread = read(new_socket, buffer, 1024);                                                                   // reading data sent by the client 
    if(valread < 0)
    {
        perror("error at reading");
        exit(EXIT_FAILURE);
    }

    cout << buffer << endl;                                                                                             

    close(new_socket);                                                                                          // closing the connected socket

    shutdown(server_fd, SHUT_RDWR);                                                                             // closing the listening socket

    return 0;
}    