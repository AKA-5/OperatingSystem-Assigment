// M Kaleem Akhtar
// 23I-0524
// CS-C

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <wait.h>


volatile int professorAvailable = 1;  // 1 mean prof is available and 0 for busy

void QuestionStart(int id) 
{
    // check if professor is availabe before asking question
    while (!professorAvailable) 
    {
        usleep(1000); 
    }
    // set for question ask
    professorAvailable = 0;
    printf("Student %d asks question.\n", id);
}

void QuestionDone(int id) 
{
    printf("Student %d gets answer\n", id);
}

void AnswerStart() 
{
    printf("Professor starts answering\n");
}

void AnswerDone() 
{
    professorAvailable = 1;  // reset professor as available
    printf("Professor ready for next question\n");
}

void* student(void* arg) 
{
    int id = *(int*)arg;
    printf("Student %d arrives\n", id);
    
    QuestionStart(id);
    sleep(1); 
    
    AnswerStart();
    sleep(1); 
    AnswerDone();
    
    QuestionDone(id);

    printf("\n");
    return NULL;
}

int main() 
{
    int n = 3;  // students count
    pthread_t students[n];
    int studentID[n];// assigning ids to keep track
    
    for (int i = 0; i < n; i++) 
    {
        studentID[i] = i+1;
    }
    
    for (int i = 0; i < n; i++) 
    {
        pthread_create(&students[i], NULL, student, &studentID[i]);
        pthread_join(students[i], NULL);   
    }
    
    printf("Office hours End.\n");
    return 0;
}

