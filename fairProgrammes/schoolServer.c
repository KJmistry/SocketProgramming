/*__________server program for managing single client__________*/ 
/** CHANGES
 * 
 * ADDED UPDATE FUNCTIONALITY FOR MANAGING DATA
 * REMOVED memset()
 * ADDED ERROR HANDLING AT send() and receive() 
 *  
*/

#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define PORT 8080
#define EXIT_FAILURE 1
#define MAX_ENTRY 10
#define RCV_BUFFER_SIZE 30
#define STUDENT_NAME_SIZE 21
#define TEMP_STRING_SIZE 20

// Macros for coloured out-put on colsole
#define GREEN "\033[0;32m"
#define CYAN "\033[0;36m"
#define YELLOW "\033[0;33m"
#define WHITE "\033[0;37m"

char null = 0;

// enum for switch cases
enum switchCases { ADD = 1, DELETE, UPDATE };

/*
    dataBase structure to store student details,
    In curret scenario: Name and Roll Number
*/
struct dataBase
{
    char studentName[STUDENT_NAME_SIZE];
    int rollNumber;
};

/*
    Takes string and Index as an input argument,
    Removes the element pointed by Index and re-arrange the string
    Returns 0 on Success and 1 on failure !
*/
int eraseElement(char *str, int deleteIndex)
{
    memmove(&str[deleteIndex], &str[deleteIndex + 1], strlen(str) - deleteIndex);
    return 0;
}

/*
    Takes list of entries and compares roll number with all the entered roll numbers assigned with each entry,
    If same roll already exists returns rollNumberIndex else return 0.
    [In summary : if unique roll number --> return -1 else return rollNumberIndex]
*/
int uniqueRollNumberValidation(int rollNumber, struct dataBase **listPtr, int listSize)
{
    int tIndex = 0;
    for (tIndex = 0; tIndex < listSize; tIndex++)
    {
        if ((*(listPtr + tIndex))->rollNumber == rollNumber)
            return tIndex;
    }
    return -1;
}

/*
    Takes received string (char *), pointer to the array of structure pointers, and offset(reference index to append new data)
    Dynamically allocates memory of size dataBase structure and stores student information inside the allocated structure
    Appends the reference (pointer to that structure) to the array of structure pointer.
    Return 0 on success andf 1 on failure.
*/
int addData(char *str, struct dataBase **listPtr, int listOffset)
{
    if (NULL == listPtr || NULL == str)
    {
        printf("NULL detected ! in function %s , line number %d\n", __func__, __LINE__);
        return 1;
    }

    int strIndex = 0;
    char tStr[TEMP_STRING_SIZE] = {0};
    struct dataBase *ptr = (struct dataBase *)malloc(sizeof(struct dataBase));

    // extracting student name 
    while (str[strIndex] != 1)
    {
        tStr[strIndex] = str[strIndex];
        strIndex++;
    }

    // storing name
    strcpy(ptr->studentName, tStr);

    int rollIndex = 0;
    
    // skipping special character ('1') 
    strIndex++;
    
    // extracting roll number
    while (str[strIndex] != 0)
    {
        tStr[rollIndex] = str[strIndex];
        strIndex++;
        rollIndex++;
    }
    // adding NULL at the end to avoid memset
    tStr[rollIndex] = 0;

    if ((listOffset > 0) && (uniqueRollNumberValidation(atoi(tStr), listPtr, listOffset) != -1))
    {
        printf("Roll number is not unique !\n");
        return 1;
    }

    // storing roll number
    ptr->rollNumber = atoi(tStr);
    *(listPtr + listOffset) = ptr;

    printf(GREEN "################## -Entry table after adding entry- #################\n");
    for (int i = 0; i <= listOffset; i++)
    {
        printf(YELLOW "------------------Entry Number: %d ----------------\n", (i + 1));
        printf(CYAN "NAME : %s\n", (*(listPtr + i))->studentName);
        printf(CYAN "ROLL NUMBER : %d\n", (*(listPtr + i))->rollNumber);
    }
    printf(GREEN "####################################################################\n" WHITE);

    return 0;
}

/*
    Takes received string (char *), pointer to the array of structure pointers, and offset(reference index to update data)
    Updates entry based on entered roll number. 
    Return 0 on success and posotive int(> 0) on failure based of failure type.
*/
int updateData(char *str, struct dataBase **listPtr, int listOffset)
{
    if (NULL == listPtr || NULL == str)
    {
        printf("NULL detected ! in function %s , line number %d\n", __func__, __LINE__);
        return 1;
    }

    int strIndex = 0, updateRoll, tStrIndex = 0;
    char tStr[TEMP_STRING_SIZE] = {0};

    // extracting roll number to search entry in data base
    while (str[strIndex] != 1)
    {
        tStr[tStrIndex] = str[strIndex];
        strIndex++;
        tStrIndex++;
    }
    tStr[tStrIndex] = 0;

    updateRoll = atoi(tStr);
    int tIndex = uniqueRollNumberValidation(updateRoll, listPtr, listOffset);
    if (tIndex == -1)
    {
        printf("No Entry found with roll number : %d\n", updateRoll);
        return 2;
    }    

    strIndex++;
    tStrIndex = 0;

    // extracting new name 
    while (str[strIndex] != 1)
    {
        tStr[tStrIndex] = str[strIndex];
        strIndex++;
        tStrIndex++;
    }
    tStr[tStrIndex] = 0;

    // updating name
    strcpy((*(listPtr + tIndex))->studentName,tStr);
    
    strIndex++;
    tStrIndex = 0;

    // extracting new roll number 
    while (str[strIndex] != 0)
    {
        tStr[tStrIndex] = str[strIndex];
        strIndex++;
        tStrIndex++;
    }
    tStr[tStrIndex] = 0;

    if((uniqueRollNumberValidation(atoi(tStr), listPtr, listOffset) !=  -1) && (atoi(tStr) != updateRoll))
    {
        printf("New roll number is not unique !\n");
        return 3;
    }

    // updating roll number
    (*(listPtr + tIndex))->rollNumber = atoi(tStr);
    
    printf(GREEN "##########-Entry table after updating roll number-%d entry-#########\n", updateRoll);
    for (int i = 0; i < listOffset; i++)
    {
        printf(YELLOW "------------------Entry Number: %d ----------------\n", (i + 1));
        printf(CYAN "NAME : %s\n", (*(listPtr + i))->studentName);
        printf(CYAN "ROLL NUMBER : %d\n", (*(listPtr + i))->rollNumber);
    }
    printf(GREEN "####################################################################\n" WHITE);
    
    return 0;
}

/*
    Takes specific index (integer), pointer to the array of structure pointers and current entries in tabel(int),
    Removes specified index entry from the array and re-arranges the array. Also displays the final table structure after modifying.
    Return 0 on success and 1 on failure.
*/
int deleteEntry(int rollNumber, struct dataBase **listPtr, int currentEntryIndex) // Function to delete specific entry
{

    if (NULL == listPtr)
    {
        printf("NULL detected ! in function %s , line number %d\n", __func__, __LINE__);
        return 1;
    }
    
    int tIndex = uniqueRollNumberValidation(rollNumber, listPtr, currentEntryIndex);
    if (tIndex == -1)
    {
        printf("No Entry found with roll number : %d\n", rollNumber);
        return 1;
    }

    for (; tIndex < currentEntryIndex; tIndex++)
    {
        *(listPtr + tIndex) = *(listPtr + tIndex + 1);
    }
    *(listPtr + tIndex) = 0;

    printf(GREEN "##########-Entry table after removing roll number-%d entry-#########\n", rollNumber);
    for (int i = 0; i < currentEntryIndex - 1; i++)
    {
        printf(YELLOW "------------------Entry Number: %d ----------------\n", (i + 1));
        printf(CYAN "NAME : %s\n", (*(listPtr + i))->studentName);
        printf(CYAN "ROLL NUMBER : %d\n", (*(listPtr + i))->rollNumber);
    }
    printf(GREEN "####################################################################\n" WHITE);
    return 0;
}

/*
    Takes char* to the message string and socket fd.
    sends the message to the destination by calling send system call,
    returns the value returened by send() system call,
*/
int sendMessage(char *message, int fd)
{
    char tBuffer[TEMP_STRING_SIZE] = {0};
    snprintf(tBuffer, TEMP_STRING_SIZE, "%s", message);
    return send(fd, tBuffer, strlen(message) + 1, 0);
}

int main(void)
{

    int serverFd, newSocket, valRead, currentEntryIndex = 0, valid, deleteRollNumber;
    struct dataBase *ptrArr[MAX_ENTRY];

    struct sockaddr_in serverAddress, clientAddress;
    int client_addrlen = sizeof(clientAddress);

    char receivedString[RCV_BUFFER_SIZE] = {0};

    // Creating socket file descriptor [IPv4 , TCP , protocol used (0 for IP)]
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket failed : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // for IPv4
    serverAddress.sin_family = AF_INET;
    // Accept req from any IP
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    // Converts a port number in host byte order to a port number in network byte order
    serverAddress.sin_port = htons(PORT);

    // Attaching socket to the port 8080
    if (bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("bind failed : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Listeinig for connection request from client (client limit = 3)
    if (listen(serverFd, 3) < 0)
    {
        printf("error at listen : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // Accepting connection request and assigning new socket fd for communication


    if ((newSocket = accept(serverFd, (struct sockaddr *)&clientAddress, (socklen_t *)&client_addrlen)) < 0)
    {
        printf("error at accept : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("Total Entries: %d\n", currentEntryIndex);
        // reading data sent by the client
        valRead = recv(newSocket, receivedString, sizeof(receivedString), 0);
        if (valRead <= 0)
        {   
            // recv failed [client disconnected]
            printf("error at reading\n");
            
            // closing the connected socket
            close(newSocket);
            
            // waiting for student client again
            if ((newSocket = accept(serverFd, (struct sockaddr *)&clientAddress, (socklen_t *)&client_addrlen)) < 0)
            {
                printf("error at accept : %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            continue;
        }

        printf("Received string from client : %s\n", receivedString);

        if (strcmp(receivedString, "close") == 0)
        {   
            // closing the connected socket
            close(newSocket);

            // waiting for student client again
            if ((newSocket = accept(serverFd, (struct sockaddr *)&clientAddress, (socklen_t *)&client_addrlen)) < 0)     
            {
                printf("error at accept : %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            continue;
        }

        if (receivedString[0] == ADD)
        {
            if (currentEntryIndex == MAX_ENTRY)
            {
                printf("No enough space in Data Base, Delete entry before adding new entry !\n");
                valid = sendMessage("No space available!", newSocket);
                if (valid <= 0)
                {
                    printf("send Failed !\n %s\n", strerror(errno));
                }
                continue;
            }

            // removing first character [request type reference]
            valid = eraseElement(receivedString, 0);
            if (1 == valid)
            {
                printf("Error ocured inside eraseElement function\n");
                valid = sendMessage("Error occured !", newSocket);
                if (valid <= 0)
                {
                    printf("send Failed !\n %s\n", strerror(errno));
                }
                continue;
            }

            valid = addData(receivedString, ptrArr, currentEntryIndex);
            if (1 == valid)
            {
                printf("Error ocured while adding data\n");
                valid = sendMessage("Error occured [check roll number]!", newSocket);
                if (valid <= 0)
                {
                    printf("send Failed !\n %s\n", strerror(errno));
                }
                continue;
            }
            printf("Entry Added for:\n");
            printf("Student Name : %s\n", ptrArr[currentEntryIndex]->studentName);
            printf("Roll Number : %d\n", ptrArr[currentEntryIndex]->rollNumber);
            currentEntryIndex++;
            valid = sendMessage("Entry Added successfully !", newSocket);
            if (valid <= 0)
            {
                printf("send Failed !\n %s\n", strerror(errno));
            }
        }

        else if (receivedString[0] == DELETE)
        {
            if (currentEntryIndex == 0)
            {
                printf("Data Base is empty, Add some entry first !\n");
                valid = sendMessage("Data base is empty !", newSocket);
                if (valid <= 0)
                {
                    printf("send Failed !\n %s\n", strerror(errno));
                }
                continue;
            }
            // removing first character [request type reference]
            valid = eraseElement(receivedString, 0);
            if (1 == valid)
            {
                printf("Error ocured inside eraseElement function\n");
                valid = sendMessage("Error occured !", newSocket);
                if (valid <= 0)
                {
                    printf("send Failed !\n %s\n", strerror(errno));
                }
                continue;
            }

            deleteRollNumber = atoi(receivedString);

            valid = deleteEntry(deleteRollNumber, ptrArr, currentEntryIndex);
            if (1 == valid)
            {
                valid = sendMessage("No Entry Found for entered roll number", newSocket);
                if (valid <= 0)
                {
                    printf("send Failed !\n %s\n", strerror(errno));
                }
                continue;
            }
            currentEntryIndex--;
            valid = sendMessage("Entry Deleted successfully !", newSocket);
            if (valid <= 0)
            {
                printf("send Failed !\n %s\n", strerror(errno));
            }
        }
        else if (receivedString[0] == UPDATE)
        {
            if (currentEntryIndex == 0)
            {
                printf("Data Base is empty, Add some entry first !\n");
                valid = sendMessage("Data base is empty !", newSocket);
                if (valid <= 0)
                {
                    printf("send Failed !\n %s\n", strerror(errno));
                }
                continue;
            }
            // removing first character [request type reference]
            valid = eraseElement(receivedString, 0);
            if (1 == valid)
            {
                printf("Error ocured inside eraseElement function\n");
                valid = sendMessage("Error occured !", newSocket);
                if (valid <= 0)
                {
                    printf("send Failed !\n %s\n", strerror(errno));
                }
                continue;
            }

            valid = updateData(receivedString, ptrArr, currentEntryIndex);
            if (0 != valid)
            {
                printf("Error occured while updating data base\n");
                if(valid == 1)
                {
                    valid = sendMessage("Error occured !", newSocket);
                    if (valid <= 0)
                    {
                        printf("send Failed !\n %s\n", strerror(errno));
                    }
                }

                if(valid == 2)
                {
                    valid = sendMessage("No entry found matching with entered roll number !", newSocket);
                    if (valid <= 0)
                    {
                        printf("send Failed !\n %s\n", strerror(errno));
                    }
                }

                if(valid == 3)
                {
                    valid = sendMessage("New roll number entered is not unique !", newSocket);
                    if (valid <= 0)
                    {
                        printf("send Failed !\n %s\n", strerror(errno));
                    }
                }
                continue;
            }
            valid = sendMessage("Data base updated successfully !", newSocket);
            if (valid <= 0)
            {
                printf("send Failed !\n %s\n", strerror(errno));
            }

        }
    }

    return 0;
}