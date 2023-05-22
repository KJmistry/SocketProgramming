#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define EXIT_FAILURE 1
#define MAX_ROLL 100
#define MIN_ROLL 0
#define MAX_NAME_LEN 20
#define STUDENT_NAME_BUFFER_SIZE 50
#define FINAL_DATA_BUFFER_SIZE 30
#define RCV_BUFFER_SIZE 1024
#define MAX_ENTRY 10
#define MIN_ENTRY 0

/*
    -Takes character pointer (where the final generated frame string will get stored),
     another character pointer to the name entered by user (that will be added in final frame),
     and integer value that cointains roll number entered by user.
    -Returns 0 on success and 1 on failure.

    [***In summary createFrame function will take data and final frame destination as arguments,
    generates the final frame in desired format and stores it to the destination***]
*/
int createFrame(char *finalData, char *name, int rollNo) // Function to create frame
{
    if (NULL == finalData || NULL == name) // NULL checking
    {
        printf("NULL detected in function %s , line number %d\n", __func__, __LINE__);
        return 1;
    }

    int finalDataIndex = 1, rollIndex = 0;
    char strRollNo[5];
    memset(strRollNo, '\n', 5);
    while (name[finalDataIndex - 1] != '\n')
    {
        finalData[finalDataIndex] = name[finalDataIndex - 1];
        finalDataIndex++;
    }

    finalData[finalDataIndex] = '*';
    finalDataIndex++;

    sprintf(strRollNo, "%d", rollNo);

    while (strRollNo[rollIndex] != '\n')
    {
        finalData[finalDataIndex] = strRollNo[rollIndex];
        finalDataIndex++;
        rollIndex++;
    }
    finalDataIndex--;
    finalData[finalDataIndex] = '\n';
    return 0;
}

int main(void)
{
    int status, valread, client_fd, switchCase, rollNo, index = 0, deleteIndex, valid;

    struct sockaddr_in serv_addr;

    char studentName[STUDENT_NAME_BUFFER_SIZE], finalData[FINAL_DATA_BUFFER_SIZE];

    char buffer[RCV_BUFFER_SIZE] = {0};

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;   // for IPv4
    serv_addr.sin_port = htons(PORT); // assigning server port number

    /* converts given string in to network address structure
       type ptovided by first argument (AF_INET --> IPv4)
       assign final output at destination pointed by 3rd argument */

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) // Trying to connect with server
    {
        perror("Connection Failed \n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        memset(finalData, 0, sizeof(finalData));
        memset(buffer, 0, sizeof(buffer));

        printf("Enter 1 : To Add Entry\n");
        printf("Enter 2 : To Delete Entry\n");
        printf("Enter 4 : To Terminate Connection\n");

        switchCase = 0;
        if (1 != scanf("%d", &switchCase))
        {
            while ((getchar()) != '\n'); // This will consume the '\n' char
            printf("Enter Valid Input\n");
            continue;
        }

        while ((getchar()) != '\n'); // This will consume the '\n' char

        switch (switchCase)
        {
        case 1:
            printf("Enter Name [Maximun characters allowed 20]\n");
            fgets(studentName, 50, stdin);

            if (strlen(studentName) > MAX_NAME_LEN + 1)
            {
                printf("Name Too Long\n");
                while ((getchar()) != '\n'); // This will consume the '\n' char
                continue;
            }

            printf("Enter Roll Number [Within range 0 to 100]\n");
            if (1 != scanf("%d", &rollNo)) // scanf validation (retures 1 on success )
            {
                while ((getchar()) != '\n'); // This will consume the '\n' char
                printf("Enter Valid Roll Number\n");
                continue;
            }

            if (rollNo > MAX_ROLL || rollNo < MIN_ROLL) // Roll Number range validation
            {
                printf("Enter Valid Roll Number\n");
                continue;
            }

            finalData[0] = '1';
            valid = createFrame(finalData, studentName, rollNo); // Calling crete frame function by providing desired argumets to generate frame
            if (1 == valid)
            {
                printf("Error occured while creting frame !");
                continue;
            }
            printf("%s\n", finalData);
            break;

        case 2:
            printf("Enter Index\n");
            if (1 != scanf("%d", &deleteIndex))
            {
                while ((getchar()) != '\n');    // This will consume the '\n' char
                printf("Enter Valid Index Number\n");
                continue;
            }
            if (deleteIndex > MAX_ENTRY || deleteIndex < MIN_ENTRY)
            {
                printf("Enter Valid Index\n");
                continue;
            }

            finalData[0] = '2';
            finalData[1] = '*';
            if (deleteIndex == 10) // special case for 10th Index input
            {
                finalData[2] = '1';
                finalData[3] = '0';
                finalData[4] = '\n';
                break;
            }
            finalData[2] = deleteIndex + '0';
            finalData[3] = '\n';
            printf("%s\n", finalData);
            break;

        // case 3:
        //     printf("Enter Index\n");
        //     scanf("%s",deleteIndex);
        //     finalData[0] = '2';
        //     finalData[1] = '*';

        //     while((deleteIndex[index] != '\n') && (index < 2))
        //     {
        //         finalData[index+2] = deleteIndex[index];
        //         index++;
        //     }
        //     printf("%s\n",finalData);
        //     break;
        case 4:
            send(client_fd, "close", 5, 0);
            close(client_fd); // closing the connected socket
            exit(0);

        default:
            printf("Enter Valid Input !\n");
            continue;
        }

        send(client_fd, finalData, sizeof(finalData), 0);
        recv(client_fd, buffer, sizeof(buffer), 0);
        printf("From server : %s\n\n", buffer);
    }

    return 0;
}