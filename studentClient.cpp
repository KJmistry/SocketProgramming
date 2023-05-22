#include<iostream>
#include<stdio.h>
#include <arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>

using namespace std; 

#define PORT 8080

void createFrame(string &finalData,string &name,int &rollNo)                                    // Function to create frame
{
    finalData.append(name);
    finalData.append("*");
    finalData.append(to_string(rollNo));
    finalData.append("\n");
}

int main()
{
    int status, valread, client_fd, K, rollNo, index;

    struct sockaddr_in serv_addr;

    // char* ack = (char*)"1Name 55";
    string studentName,finalData;

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
        perror("Invalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)     // Trying to connect with server 
    {
        perror("Connection Failed \n");
        exit(EXIT_FAILURE);
    }   

    while(1)
    {   
        printf("Enter 1 : To Add Entry\n");
        printf("Enter 2 : To Delete Entry\n");

        cin >> K;

        switch (K)
        {
        case 1:
            printf("Enter Name\n");
            cin.ignore();
            getline(cin,studentName);
            printf("Enter Roll Number\n");
            cin >> rollNo;
            finalData = "1";
            createFrame(finalData,studentName,rollNo);
            break;

        case 2:
            printf("Enter Index\n");
            cin >> index;
            finalData = "2";
            finalData.append("*");
            finalData.append(to_string(index));
            break;

        }        
        cout << buffer << endl;

        write(client_fd, finalData.c_str(), finalData.size());
    }
    close(client_fd);                                                                           // closing the connected socket

    return 0;                                                                                       
}