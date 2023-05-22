/** CHANGES
 * 
 * ADDED UPDATE OPTION
 * REMOVED memset()
 * FUNCTIONS FOR FRAME CREATION REPLACED WITH snprintf()
 * ADDED ERROR HANDLING AT send() and receive()
 * OPTIMIZED MAX_NAME_LEN AND FINAL_DATA_BUFFER_SIZE
 * Added enum as a reference for each case (operation to be performed)
 *  
*/


#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define EXIT_FAILURE 1
#define MAX_ROLL 100
#define MIN_ROLL 1
#define MAX_NAME_LEN 21
#define FINAL_DATA_BUFFER_SIZE 30
#define RCV_BUFFER_SIZE 100
#define MAX_ENTRY 10
#define MIN_ENTRY 0

enum switchCases { ADD = 1, DELETE, UPDATE, TERMINATE };

char specialCharacter = 1, null = 0;

int main(void)
{
    int status, clientFd, switchCase, rollNo, deleteRollNum, updateRollNum, valid;

    struct sockaddr_in servAddr;

    char studentName[MAX_NAME_LEN + 1], finalData[FINAL_DATA_BUFFER_SIZE];

    char buffer[RCV_BUFFER_SIZE] = {0};

    if ((clientFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // for IPv4
    servAddr.sin_family = AF_INET;
    // assigning server port number
    servAddr.sin_port = htons(PORT);

    /* converts given string in to network address structure
       type ptovided by first argument (AF_INET --> IPv4)
       assign final output at destination pointed by 3rd argument */

    if (inet_pton(AF_INET, SERVER_IP, &servAddr.sin_addr) <= 0)
    {
        printf("Invalid address/ Address not supported : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Trying to connect with server
    if ((status = connect(clientFd, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0)
    {
        printf("Connection Failed : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1)
    {

        printf("Enter 1 : To Add Entry\n");
        printf("Enter 2 : To Delete Entry\n");
        printf("Enter 3 : To Update Entry\n");
        printf("Enter 4 : To Terminate Connection\n");

        switchCase = 0;
        if (1 != scanf("%d", &switchCase))
        {
            // This will consume the '\n' char
            while ((getchar()) != '\n');

            printf("Enter Valid Input\n");
            continue;
        }
        // This will consume the '\n' char
        while ((getchar()) != '\n');

        switch (switchCase)
        {
        case ADD:
            
            /**
             * Case 1: Add entry
             * Frame structure : [ 1(operation type) ] - [ student name ] - [specialCharacter] - [ Roll Number ] - ['0'] 
             * specialCharacter = '1' (used as a field separator) 
            */
            
            printf("Enter Name [Maximun characters allowed 20]\n");
            fgets(studentName, MAX_NAME_LEN + 1, stdin);
            studentName[MAX_NAME_LEN] = 0;
            // replacing '\n' with '0'
            studentName[strcspn(studentName, "\n")] = 0;

            if (strlen(studentName) > MAX_NAME_LEN - 1)
            {
                printf("Name Too Long\n");
                // This will consume the '\n' char
                while ((getchar()) != '\n');
                continue;
            }

            printf("Enter Roll Number [Within range 0 to 100]\n");
            // scanf validation (retures 1 on success)
            if (1 != scanf("%d", &rollNo))
            {
                // This will consume the '\n' char
                while ((getchar()) != '\n');
                printf("Enter Valid Roll Number\n");
                continue;
            }
            // Roll Number range validation
            if (rollNo > MAX_ROLL || rollNo < MIN_ROLL)
            {
                printf("Enter Valid Roll Number\n");
                continue;
            }

            // Calling crete frame function by providing desired argumets to generate frame
            snprintf(finalData, FINAL_DATA_BUFFER_SIZE, "1%s%c%d%c", studentName, specialCharacter, rollNo, null);
            
            printf("Final Frame Generated : %s\n", finalData);
            break;

        case DELETE:

            /**
             * Case 2: Delete entry by roll number
             * Frame structure : [ 2(operation type) ] - [ Roll Number(to search for entry) ] - ['0'] 
             * specialCharacter = '1' (used as a field separator) 
            */

            printf("Enter Roll Number\n");
            if (1 != scanf("%d", &deleteRollNum))
            {
                // This will consume the '\n' char
                while ((getchar()) != '\n');
                printf("Enter Valid Roll Number\n");
                continue;
            }
            while ((getchar()) != '\n');

            if (deleteRollNum > MAX_ROLL || deleteRollNum < MIN_ROLL)
            {
                printf("Enter Valid Roll Number\n");
                continue;
            }

            snprintf(finalData, FINAL_DATA_BUFFER_SIZE, "2%d%c", deleteRollNum, null);
            printf("Final Frame Generated : %s\n", finalData);
            break;

        case UPDATE:

            /**
             * Case 3: Update data searching by roll number
             * Frame structure : [ 3(operation type) ] - [ Roll Number(to search for entry) ] - [specialCharacter] -
                                 [ New Name ] -[specialCharacter] - [ New Roll Number ] - ['0'] 
             * specialCharacter = '1' (used as a field separator) 
            */

            printf("Enter Roll Number\n");
            if (1 != scanf("%d", &updateRollNum))
            {
                // This will consume the '\n' char
                while ((getchar()) != '\n');
                printf("Enter Valid Roll Number\n");
                continue;
            }
            while ((getchar()) != '\n');

            if (updateRollNum > MAX_ROLL || updateRollNum < MIN_ROLL)
            {
                printf("Enter Valid Roll Number\n");
                continue;
            }

            printf("Enter New Name [Maximun characters allowed 20]\n");
            fgets(studentName, MAX_NAME_LEN + 1, stdin);
            studentName[MAX_NAME_LEN] = 0;
            // replacing '\n' with '0'
            studentName[strcspn(studentName, "\n")] = 0;

            if (strlen(studentName) > MAX_NAME_LEN - 1)
            {
                printf("Name Too Long\n");
                // This will consume the '\n' char
                while ((getchar()) != '\n');
                continue;
            }

            printf("Enter New Roll Number [Within range 0 to 100]\n");
            // scanf validation (retures 1 on success)
            if (1 != scanf("%d", &rollNo))
            {
                // This will consume the '\n' char
                while ((getchar()) != '\n');
                printf("Enter Valid Roll Number\n");
                continue;
            }
            // Roll Number range validation
            if (rollNo > MAX_ROLL || rollNo < MIN_ROLL)
            {
                printf("Enter Valid Roll Number\n");
                continue;
            }

            snprintf(finalData, FINAL_DATA_BUFFER_SIZE, "3%d%c%s%c%d%c", updateRollNum, specialCharacter, studentName, specialCharacter, rollNo, null);
            printf("Final Frame Generated : %s\n", finalData);
            break;

        case TERMINATE:

            valid = send(clientFd, "close", 5, 0);
            if (valid < 0)
            {
                printf("send Failed !\n%s\n", strerror(errno));
            }
            // closing the connected socket
            close(clientFd);
            exit(0);

        default:
            printf("Enter Valid Input !\n");
            continue;
        }

        valid = send(clientFd, finalData, strlen(finalData) + 1, 0);

        if (valid <= 0)
        {
            printf("send Failed !\nerrno : %d %s\n", errno, strerror(errno));
            continue;
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