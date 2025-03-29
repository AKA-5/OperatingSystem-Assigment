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

// to store info of process
typedef struct
{
    int pid;
    int arrival;
    int burst;
    int remaining;
    int completion;
    int turnaround;
    int waiting;
}Process;

void sortProcesses(Process p[], int n)
{
    //bubble sort
    for(int i=0; i<n-1; i++)
    {
        for(int j = i+1; j<n; j++)
        {
            if(p[i].arrival > p[j].arrival)
            {
                //swap
                Process temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }
    return;
}

void roundRobin(Process q1[], int* s1, int quantum, int* ctime, Process q2[], int* s2)
{
    for(int i=0; i<*s1; i++)
    {

        // If CPU goes to idle state it jump to next process arrival time
        if (*ctime < q1[i].arrival)
        {
         *ctime = q1[i].arrival;
        }
        printf("--> Process[%d] ", q1[i].pid);
       
        if(q1[i].remaining > quantum)
        {
            *ctime += quantum;
            q1[i].remaining -= quantum;
            q2[*s2] = q1[i]; // moving process to next queue
            ++(*s2);
        }
        else if(q1[i].remaining > 0)
        {
            *ctime += q1[i].remaining;
            q1[i].remaining = 0;
            q1[i].completion = *ctime;
        }
    }

    return;
}


void FCFS(Process q3[], int s3, int* ctime)
{
    for(int i=0; i<s3; ++i)
    {
        // If CPU goes to idle state it jump to next process arrival time
        if (*ctime < q3[i].arrival)
        {
            *ctime = q3[i].arrival;
        }
        printf("--> Process[%d] ", q3[i].pid);
        *ctime += q3[i].remaining;
        q3[i].remaining = 0;
        q3[i].completion = *ctime;
    }

    return;
}

void printProcess(Process p[], int n)
{
    printf("\nFinal Result:\n ");
    for(int i=0; i<n; i++)
    {
        p[i].turnaround = p[i].completion - p[i].arrival;
        p[i].waiting = p[i].turnaround - p[i].burst;
        printf("PID: %d\tArrival: %d\tBurst: %d\t Complete: %d\tTAT: %d\tWT: %d\n", p[i].pid, p[i].arrival, p[i].burst, p[i].completion, p[i].turnaround, p[i].waiting);
        //printf("\nProcess %d --> Turnaround Time: %d, Waiting Time: %d \n",i+1, p[i].turnaround, p[i].waiting);
    }
    return;
}



int main()
{
    int noOfprocess = 0;

    printf("MLFQ Program");
    printf("\nInput total process: ");
    scanf("%d",&noOfprocess);

    // in order to maintain three queues
    Process q1[noOfprocess],q2[noOfprocess],q3[noOfprocess];
    int s1 = 0,s2 = 0,s3 =0;

    Process processes[noOfprocess];

    for(int i=0; i<noOfprocess; ++i)
    {
        printf("--> Process %d Input PID, Arrival Time and Burst Time: ",i+1);
        scanf("%d %d %d", &processes[i].pid, &processes[i].arrival, &processes[i].burst);
        processes[i].remaining = processes[i].burst;
    }
    // sorting by arribal time
    sortProcesses(processes, noOfprocess);

printf("\nSorted Processes (by Arrival Time):\n");
printf("PID\tArrival\tBurst\n");
for(int i = 0; i < noOfprocess; i++)
{
    printf("%d\t%d\t%d\n", processes[i].pid, processes[i].arrival, processes[i].burst);
}

    // copying into first array
    for(int i=0; i<noOfprocess; ++i)
    {
        q1[s1++] = processes[i];
    }

    int ctime = processes[0].arrival;   // setting time from first process

    printf("\nExecution Order of Process(s): \n");

    // Queue 1
    roundRobin(q1,&s1, 4, &ctime, q2, &s2);

    // Queue 2
    roundRobin(q2,&s2, 8, &ctime, q3, &s3);

    // Queue 3
    FCFS(q3, s3, &ctime);

    for(int i = 0; i<noOfprocess ; ++i)
    {
        for(int j=0; j<s1;++j)
        {
            if(processes[i].pid == q1[j].pid)
            {
                processes[i] = q1[j];
            }
        }

        for(int j=0; j<s2;++j)
        {
            if(processes[i].pid == q2[j].pid)
            {
                processes[i] = q2[j];
            }
        }

        for(int j=0; j<s3;++j)
        {
            if(processes[i].pid == q3[j].pid)
            {
                processes[i] = q3[j];
            }
        }
    }

    // print result
    printProcess(processes, noOfprocess);
    printf("\n\n");

    return 0;
}