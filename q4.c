// M Kaleem Akhtar
// 23i-0524
// CS-C

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/fcntl.h>
#include<sys/errno.h>
#include<sys/stat.h>

int main()
{   
    int pipeAB[2], pipeBC[2],pipeCD[2],pipeDA[2];

    if(pipe(pipeAB) == -1)
    {
        printf("Failed to create pipe A-->B\n");
    }
    if(pipe(pipeBC) == -1)
    {
        printf("Failed to create pipe B-->C\n");
    }
    if(pipe(pipeCD) == -1)
    {
        printf("Failed to create pipe C-->D\n");
    }
    if(pipe(pipeDA) == -1)
    {
        printf("Failed to create pipe D-->A\n");
    }

    pid_t pidA = fork();
    if(pidA == 0)
    {
        // process A
        //read message from user
        char msg[500];
        printf("\nEnter message to pass: ");
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg,"\n")] = 0;  // removes newline character


        //open file to store message
        remove("log_A.txt"); // removes if file already exisits
        int logA = open("log_A.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (logA == -1) 
        {
            perror("Error opening log_A.txt");
            exit(EXIT_FAILURE);
        }
        write(logA, msg, strlen(msg));
        close(logA);

        write(pipeAB[1], msg, strlen(msg) + 1);
        close(pipeAB[1]);

        // reading message after processing
        char lastMsg[500];
        close(pipeDA[1]);
        read(pipeDA[0], lastMsg, sizeof(lastMsg));
        printf("\nFinal Message is: %s\n", lastMsg);

        close(pipeDA[0]);
        exit(0);
    }

    // process B
    pid_t pidB = fork();
    if(pidB == 0)
    {
        close(pipeAB[1]);

        char msgB[500];
        read(pipeAB[0], msgB, sizeof(msgB));

        strcat(msgB, " - Processed by B ");
        remove("log_B.txt"); // removes if file already exisits

        int logB = open("log_B.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (logB == -1) 
        {
            perror("Error opening log_B.txt");
            exit(EXIT_FAILURE);
        }
        write(logB, msgB, strlen(msgB));
        close(logB);

        write(pipeBC[1], msgB, strlen(msgB) + 1);
        close(pipeAB[0]);
        close(pipeBC[1]);

        exit(0);
    } 

    // process C
    pid_t pidC = fork();
    if(pidC == 0)
    {
        close(pipeBC[1]);

        char msgC[500];
        read(pipeBC[0], msgC, sizeof(msgC));
        strcat(msgC, " - Processed by C ");
        remove("log_C.txt"); // removes if file already exisits

        int logC = open("log_C.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (logC == -1) 
        {
            perror("Error opening log_C.txt");
            exit(EXIT_FAILURE);
        }
        write(logC, msgC, strlen(msgC));
        close(logC);

        write(pipeCD[1], msgC, strlen(msgC) + 1);
        close(pipeBC[0]);
        close(pipeCD[1]);

        exit(0);

    }

    // process D
    pid_t pidD = fork();
    if(pidD == 0)
    {
        close(pipeCD[1]);

        char msgD[500];
        read(pipeCD[0], msgD, sizeof(msgD));
        strcat(msgD, " - Processed by D ");
        remove("log_D.txt"); // removes if file already exisits

        int logD = open("log_D.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (logD == -1) 
        {
            perror("Error opening log_D.txt");
            exit(EXIT_FAILURE);
        }
        write(logD, msgD, strlen(msgD));
        close(logD);

        write(pipeDA[1], msgD, strlen(msgD) + 1);
        close(pipeCD[0]);
        close(pipeDA[1]);

        exit(0);

    } 
    
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("\nEND OF PROGRAM\n");
    return 0;
}