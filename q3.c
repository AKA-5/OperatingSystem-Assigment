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
#include<math.h>
#include<stdbool.h>

// structs
typedef struct
{
    int id;
    char name[50];
    float battingAvg;
    float strikeRate;
    float bowlingAvg;
    float economy;
    float currentForm;
}Player;

typedef struct 
{
    int playerCount;
    Player* players;
}Team;

void inputData(Team* team)
{
    for(int i=0; i<team->playerCount; ++i)
    {
        printf("\nPlayer %d Info:",i+1);
        team->players[i].id = i+1;

        printf("\nEnter Name: ");
        scanf("%s", team->players[i].name);
        printf("\nEnter Batting Avg: ");
        scanf("%f", &team->players[i].battingAvg);
        printf("\nEnter Strike Rate: ");
        scanf("%f", &team->players[i].strikeRate); 
        printf("\nEnter Bowling Avg: ");
        scanf("%f", &team->players[i].bowlingAvg); 
        printf("\nEnter Economy Rate: ");
        scanf("%f", &team->players[i].economy);
        printf("\nEnter Current Form (0-100): ");
        scanf("%f", &team->players[i].currentForm);

    }
    return;
}

// compares strike rate
int compareSJF(const void* player1, const void* player2)
{
    Player* p1 = (Player*) player1;
    Player* p2 = (Player*) player2;
    if(p2->strikeRate > p1->strikeRate)
    {
        return 1;
    }
    return -1;
}


int compareRR(const void* player1, const void* player2)
{
    Player* p1 = (Player*) player1;
    Player* p2 = (Player*) player2;
    if(p2->economy > p1->economy)
    {
        return 1;
    }
    return -1;
}

void battingOrderSJF(Team* team)
{
    Player* sorted = (Player*)malloc(team->playerCount * sizeof(Player));
    memcpy(sorted, team->players, team->playerCount * sizeof(Player));  // copy from team players

    qsort(sorted, team->playerCount, sizeof(Player), compareSJF);
    printf("\nBatting Order (High Strike Rate):");

    for(int i=0; i<team->playerCount;++i)
    {
        printf("\n%d. Name: %s Striking Rate: %.2f",i+1, sorted[i].name, sorted[i].strikeRate);
    }


    free(sorted);
    return;
}

// batting order based on strike rate and form
void battingOrderSRTF(Team* team)
{
    Player* sorted = (Player*)malloc(team->playerCount * sizeof(Player));
    memcpy(sorted, team->players, team->playerCount * sizeof(Player));  // copy from team players

    for(int i=0; i<team->playerCount - 1; ++i)
    {
        for(int j=i+1; j<team->playerCount;++j)
        {
            float s1 = sorted[i].strikeRate * (sorted[i].currentForm / 100.0);
            float s2 = sorted[j].strikeRate * (sorted[j].currentForm / 100.0);
            if(s2 > s1)
            {
                Player temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }

        }
    }

    printf("\nBatting Order (SRTF):");

    for(int i=0; i<team->playerCount;++i)
    {
        printf("\n%d. Name: %s Score Rate: %.2f",i+1, sorted[i].name, (sorted[i].strikeRate * (sorted[i].currentForm / 100.0)));
    }

    free(sorted);
    return;

}

void bowlingOrderRR(Team* team)
{
    int bowlersCount=0;
    for(int i=0; i<team->playerCount; ++i)
    {
        if(team->players[i].bowlingAvg > 0)
        {
            ++bowlersCount;
        }
    }

    Player* bowler = (Player*)malloc(bowlersCount * sizeof(Player));
    int count=0;
    for(int i=0; i<team->playerCount; ++i)
    {
        if(team->players[i].bowlingAvg > 0)
        {
            bowler[count++] = team->players[i];
        }
    }

    qsort(bowler, bowlersCount, sizeof(Player), compareRR);
    
    printf("\nBowling Order (RR):\n");

    for(int i=0; i<bowlersCount;++i)
    {
        printf("\n%d. Name: %s Economy Rate: %.2f",i+1, bowler[i].name, bowler[i].economy);
    }

    free(bowler);
    

    return;
}

void match(Team* team)
{

    printf("\nMatch - 20 overs\n");

    Player* batters = (Player* )malloc (team->playerCount * sizeof(Player));
    memcpy(batters, team->players, team->playerCount * sizeof(Player));
    qsort(batters, team->playerCount, sizeof(Player), compareSJF);

    int totalRuns = 0;
    int remBalls = 120;
    int wicket =0;

    for(int i=0; i<team->playerCount && remBalls > 0; ++i)
    {
        int ballsPlayed = (remBalls > 30) ? 30: remBalls;
        int runs = batters[i].strikeRate * ballsPlayed / 100;
        totalRuns += runs;
        remBalls -= ballsPlayed;
        wicket++;

        printf("%s scored %d runs in %d balls.\n", batters[i].name, runs, ballsPlayed);

    }

    printf("\nFinal Score: %d runs \nWickets: %d", totalRuns, wicket - 1);
    free(batters);


    return;
}



int main()
{
    Team team;
    printf("Enter number of players: ");
    scanf("%d", &team.playerCount);

    team.players = (Player*)malloc(team.playerCount * sizeof(Player));  // DMA

    inputData(&team);


    printf("\nBatting Order (SJF):\n");
    battingOrderSJF(&team);
    battingOrderSRTF(&team);
    bowlingOrderRR(&team);
    match(&team);



    free(team.players);

    return 0;
}