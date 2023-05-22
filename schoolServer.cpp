#include<iostream>
#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include <list>
#include <vector>
#include <sstream>

using namespace std; 

#define PORT 8080

struct dataBase
{   
    string studentName;
    int rollNmber;

    void printData()
    {
        cout << "Name : "<< studentName << endl;
        cout << "Roll Number : "<< rollNmber << endl;
    } 
};


void addData(list <dataBase*> &listRef, string &data)
{   
    dataBase* ptr = new dataBase;
    
    string delimiter = "*";
    size_t pos = 0;
    string token;

    while ((pos = data.find(delimiter)) != std::string::npos) 
    {
        token = data.substr(0, pos);
        ptr->studentName = token;
        data.erase(0, pos + delimiter.length());
    }
    ptr->rollNmber = atoi(data.c_str());
    
    cout << "Data Added : " << endl;
    ptr->printData();
    cout << endl;

    listRef.emplace_back(ptr);

    return;
}

void deleteData(list <dataBase*> &listRef, string &data)
{   
    int offset = atoi(data.c_str());
    auto index = listRef.begin();
    advance(index,offset-1);
    listRef.erase(index);

    printf("data base after deleting index %d entry\n\n", offset);
    
    for(auto i : listRef)
    {
        i->printData();
        cout << endl;
    }
}

int main()
{
    list <dataBase*> studentList;

    int server_fd, new_socket, valread;

    struct sockaddr_in server_address,client_address;
    int client_addrlen = sizeof(client_address);
        
    char buffer[1024] = { 0 };
    string receivedString;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)                                                           // Creating socket file descriptor [IPv4 , TCP , protocol used (0 for IP)]
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;                                                                            // for IPv4                                                   
    server_address.sin_addr.s_addr = INADDR_ANY;                                                                    // IP of host machine on which server is running (always set to own IP --> INADDR_ANY)
    server_address.sin_port = htons(PORT);                                                                          // Converts a port number in host byte order to a port number in network byte order 
                                                                                                                    // ex: 0x1389 will be stored as --> 0x89 0x13

    if(bind(server_fd, (struct sockaddr*)&server_address,sizeof(server_address)) < 0)                               // Attaching socket to the port 8080
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 3) < 0)                                                                                    // Listeinig for connection request from client (client limit = 3)
    {
        perror("error at listen");
        exit(EXIT_FAILURE);
    }

    if((new_socket = accept(server_fd, (struct sockaddr*)&client_address, (socklen_t*)&client_addrlen)) < 0)        // Accepting connection request and assigning new socket fd for communication
    {
        perror("error at accept");
        exit(EXIT_FAILURE);
    }
    while(1)
    { 
        
        memset(buffer,0,sizeof(buffer));                                                                            // clearing buffer before storing data

        valread = read(new_socket, buffer, 1024);                                                                   // reading data sent by the client 
        if(valread < 0)
        {
            perror("error at reading");
            exit(EXIT_FAILURE);
        }
        cout << "Frame received : "; 
        cout << buffer << endl;
        cout << endl;

        receivedString = buffer;

        if(receivedString.at(0) == '1')
        {
            receivedString.erase(0,1);
            addData(studentList,receivedString);
        }
        else if(receivedString.at(0) == '2')
        {
            receivedString.erase(0,2);
            deleteData(studentList,receivedString);
        }
    }

    close(new_socket);                                                                                              // closing the connected socket

    shutdown(server_fd, SHUT_RDWR);                                                                                 // closing the listening socket

    return 0;
}    