#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

#define PORT 8080
#define EXIT_FAILURE 1
#define MAX_ENTRY 10
#define RCV_BUFFER_SIZE 1024
#define STUDENT_NAME_SIZE 20
#define TEMP_STRING_SIZE 50

// Macros for coloured out-put on colsole
#define GREEN "\033[0;32m"
#define CYAN "\033[0;36m"
#define YELLOW "\033[0;33m"
#define WHITE "\033[0;37m"


struct dataBase
{
    char studentName[STUDENT_NAME_SIZE];
    int rollNumber;
};

int getStrLen(char *str)
{
    int temp = 0;
    while(str[temp] != '\n')
    {
        temp++;
    }
    return temp;
}

/*
    Takes string and Index as an input argument,
    Removes the element pointed by Index and re-arrange the string
    Returns 0 on Success and 1 on failure !
*/
int eraseElement(char* str, int deleteIndex)                                                        // Function to erase specific index element
{   
    int strlen;
    char *tStr = (char*)malloc(TEMP_STRING_SIZE);

    strlen = getStrLen(str); 
    strcpy(tStr,str);
    if(strlen < deleteIndex)
    {
        printf("Index Invalid\n");
        return 1;
    }

    for( deleteIndex ; deleteIndex < strlen ; deleteIndex++)
    {
        str[deleteIndex] = tStr[deleteIndex + 1];
    }
    str[deleteIndex] = '\b';
    return 0;
}

/*
    Takes received string (char *), pointer to the array of structure pointers, and offset(reference index to append new data)
    Dynamically allocates memory of size dataBase structure and stores student information inside the allocated structure
    Appends the reference (pointer to that structure) to the array of structure pointer.
    Return 0 on success andf 1 on failure. 
*/
int addData(char *str, struct dataBase **listPtr, int listOffset)                                  // Function to add entry in student database
{
    if(NULL == listPtr || NULL == str)
    {
        printf("NULL detected ! in function %s , line number %d\n",__func__,__LINE__);
        return 1;
    }

    int strIndex = 0;
    char tStr[TEMP_STRING_SIZE] = { 0 }; 
    struct dataBase *ptr = (struct dataBase*)malloc(sizeof(struct dataBase));
    while(str[strIndex] != '*')
    {
        tStr[strIndex] = str[strIndex];
        strIndex++; 
    }
    strcpy(ptr->studentName,tStr);
    
    int rollIndex = 0;
    memset(tStr,0,TEMP_STRING_SIZE);
    
    strIndex++;

    while(str[strIndex] != '\n')
    {
        tStr[rollIndex] = str[strIndex];
        strIndex++;
        rollIndex++; 
    }
    ptr->rollNumber = atoi(tStr);
    *(listPtr + listOffset) = ptr;
    return 0;
}

/*
    Takes specific index (integer), pointer to the array of structure pointers and current entries in tabel(int),
    Removes specified index entry from the array and re-arranges the array. Also displays the final table structure after modifying.
    Return 0 on success andf 1 on failure. 
*/
int deleteEntry(int index, struct dataBase **listPtr,int currentEntryIndex)                         // Function to delete specific entry 
{   

    if(NULL == listPtr)
    {
        printf("NULL detected ! in function %s , line number %d\n",__func__,__LINE__);
        return 1;
    }

    int tIndex = index;
    index--;
   
    struct dataBase* tPtr;
    if(NULL == tPtr)
    {
        printf("Unable to alocate memory (NULL detected !) in function %s , line number %d\n",__func__,__LINE__);
        return 1;
    }

    for(index ; index < currentEntryIndex ; index++)
    {
        *(listPtr + index) = *(listPtr + index + 1);
    }
    *(listPtr + index) = 0;

    printf(GREEN"##########-Entry table after removing index-%d entry-#########\n",tIndex);
    for(int i = 0; i < currentEntryIndex-1; i++)
    {   
        printf(YELLOW"------------------Entry Number: %d ----------------\n",(i+1));
        printf(CYAN"%s\n",(*(listPtr+i))->studentName);
        printf(CYAN"%d\n",(*(listPtr+i))->rollNumber);
    }
    printf(GREEN"##############################################################\n"WHITE);
    return 0;
}

int main()
{

    int server_fd, new_socket, valread, currentEntryIndex = 0, valid, deleteIndex;
    struct dataBase *ptrArr[MAX_ENTRY];

    struct sockaddr_in server_address,client_address;
    int client_addrlen = sizeof(client_address);
        
    char receivedString[RCV_BUFFER_SIZE] = { 0 };
    
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)                                                           // Creating socket file descriptor [IPv4 , TCP , protocol used (0 for IP)]
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;                                                                            // for IPv4                                                   
    server_address.sin_addr.s_addr = INADDR_ANY;                                                                    // Accept req from any IP 
    server_address.sin_port = htons(PORT);                                                                          // Converts a port number in host byte order to a port number in network byte order 

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
        printf("Total Entries: %d\n",currentEntryIndex);
        memset(receivedString,0,sizeof(receivedString));                                                             // clearing buffer before storing data

        valread = recv(new_socket, receivedString, sizeof(receivedString), 0);                                       // reading data sent by the client 
        if(valread < 0)
        {
            perror("error at reading");
            exit(EXIT_FAILURE);
        }
        
        printf("Received string from client : %s\n",receivedString);

        if(strcmp(receivedString,"close") == 0)
        {
            break;
        }

        if(receivedString[0] == '1')
        {   
            if(currentEntryIndex == MAX_ENTRY)
            {   
                printf("No enough space in Data Base, Delete entry before adding new entry !\n");
                send(new_socket,"No space available!", 19, 0);

                continue;
            }
            valid = eraseElement(receivedString,0);                                                                  // removing first character
            if(1 == valid)
            {   
                printf("Error ocured inside eraseElement function\n");
                send(new_socket,"Error occured !", 15, 0);
                continue;
            }

            valid = addData(receivedString,ptrArr,currentEntryIndex);
            if(1 == valid)
            {   
                printf("Error ocured while adding data\n");
                send(new_socket,"Error occured !", 15, 0);
                continue;
            }
            printf("Entry Added for:\n");
            printf("Student Name : %s\n",ptrArr[currentEntryIndex]->studentName);  
            printf("Roll Number : %d\n",ptrArr[currentEntryIndex]->rollNumber);
            currentEntryIndex++;
            send(new_socket,"Entry Added successfully !", 26, 0);
                         
        }

        else if(receivedString[0] == '2')
        {
            if(currentEntryIndex == 0)
            {   
                printf("Data Base is empty, Add some entry first !\n");
                send(new_socket,"Data base is empty !", 20, 0);
                continue;
            }
            valid = eraseElement(receivedString,0);                                                                             // removing first character
            if(1 == valid)
            {   
                printf("Error ocured inside eraseElement function\n");
                send(new_socket,"Error occured !", 15, 0);
                continue;
            }

            valid = eraseElement(receivedString,0);                                                                             // removing '*'
            if(1 == valid)
            {   
                printf("Error ocured inside eraseElement function\n");
                send(new_socket,"Error occured !", 15, 0);
                continue;
            }

            deleteIndex = atoi(receivedString);
            if(deleteIndex > currentEntryIndex)
            {
                send(new_socket,"Enter valid index !", 19, 0);
                continue;
            }                                                                         
            valid = deleteEntry(deleteIndex,ptrArr,currentEntryIndex);
            if(1 == valid)
            {
                send(new_socket,"Error occured while deleting entry !", 36, 0);
                continue;
            }
            currentEntryIndex--;
            send(new_socket,"Entry Deleted successfully !", 28, 0);

        }
    }

    close(new_socket);                                                                                              // closing the connected socket
    shutdown(server_fd, SHUT_RDWR);                                                                                 // closing the listening socket

    for(int i = 0; i < currentEntryIndex; i++)
    {
        free(*(ptrArr+i));
    }
    printf("Terminating Connection !\n");
    return 0;
}    