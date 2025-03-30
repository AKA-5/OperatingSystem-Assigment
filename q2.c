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
#include<pthread.h>


int sp[2];  // student to professor
int ps[2];  // professor to student

void QuestionStart(int id)
{
    char msg = 'q';
    printf("Student %d asked question.\n",id);
    write(sp[1], &msg, 1);  // tells student is asking question
    read(ps[0], &msg, 1);   // wait till professor start answering
}

void QuestionDone(int id)
{
    char msg = 'd';
    // write(sp[1], &msg, 1);  // tells the question ended
    // printf("Student %d question DONE.\n",id);
    read(ps[0], &msg, 1);   // ensures professor answers

}

void AnswerStart()
{
    char msg;
    read(sp[0],&msg, 1);    // professor wait until someone asks question
    printf("Professor start answering.\n");
    write(ps[1], &msg, 1); // professor responds to question
}

void AnswerDone()
{
    char msg = 'A';
    write(ps[1],&msg, 1);   // professor tells student asnwer is done
}


void* studentThread(void *arg)
{
    char msg;
    int id = *(int*)arg;
    printf("Student %d arrived.\n",id);

    sleep(rand() % 2);  // if multiple students arrive

    QuestionStart(id);
    sleep(1);
    QuestionDone(id);

    return NULL;
}


void* professorThread(void* arg)
{
    while(1)
    {
        printf("Zzzzzzz Professor is Sleeping.\n");
        AnswerStart();
        sleep(1);
        AnswerDone();
        printf("\n");
    }

    return NULL;
}

int main()
{
    srand(time(NULL));    // to generate random number

    int noOfStudent = 2;
    int stdId[noOfStudent];
    for(int i=0; i<noOfStudent; ++i)
    {
        stdId[i] = i+1;
    }

    pipe(sp);
    pipe(ps);

    pthread_t prof;
    pthread_t std[noOfStudent];

    pthread_create(&prof, NULL, professorThread, NULL);
    
    for(int i=0; i<noOfStudent; ++i)
    {
        pthread_create(&std[i], NULL, studentThread, &stdId[i]);
    }

    for(int i=0; i<noOfStudent; ++i)
    {
        pthread_join(std[i], NULL);
    }

    return 0;
}