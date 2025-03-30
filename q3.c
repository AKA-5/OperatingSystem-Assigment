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
    int bowlerCount;
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
        printf("Enter Batting Avg (0-200): ");
        scanf("%f", &team->players[i].battingAvg);
        printf("Enter Strike Rate (0-200): ");
        scanf("%f", &team->players[i].strikeRate); 
        printf("Enter Bowling Avg (0-200): ");
        scanf("%f", &team->players[i].bowlingAvg); 
        printf("Enter Economy Rate (0-20): ");
        scanf("%f", &team->players[i].economy);
        printf("Enter Current Form (0-100): ");
        scanf("%f", &team->players[i].currentForm);

        if(team->players[i].bowlingAvg > 0)
        {
            team->bowlerCount++;
        }
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
    if(p1->economy > p2->economy)
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

    printf("\nBatting Order (SRTF based on FORM):");

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
    
    printf("\nBowling Order (RR High Ecnomy):\n");

    for(int i=0; i<bowlersCount;++i)
    {
        printf("\n%d. Name: %s Economy Rate: %.2f",i+1, bowler[i].name, bowler[i].economy);
    }

    free(bowler);
    

    return;
}

void report(int sjfRuns, int srtfRuns)
{
    printf("\nReport:");
    printf("\nSJF Score: %d VS SRTF Score: %d", sjfRuns,srtfRuns);

    printf("\nRecommend: ");
    if(srtfRuns > sjfRuns)
    {
        printf("\nUse SRTF batting order. Better by %d runs As this team has overall better strike rate and form.", srtfRuns - sjfRuns);
    }
    else
    {
        printf("\nUse SJF batting order. Better by %d runs As this team has better strike rate.",sjfRuns - srtfRuns);
    }

    printf("\nFor Bowling:");
    printf("\n1. Rotate bowlers every 2-4 overs.");
    printf("\n2. Best economy bowler should bowl main overs.\n");

    return;
}


void match(Team* team)
{

    printf("\n\nMatch Simulation - 20 overs\n");

    // sjf
    Player* sjfbatters = (Player* )malloc (team->playerCount * sizeof(Player));
    memcpy(sjfbatters, team->players, team->playerCount * sizeof(Player));
    qsort(sjfbatters, team->playerCount, sizeof(Player), compareSJF);


    int sfjRuns = 0;
    int ballsLeft = 120;
    int sjfwicket =0;
    printf("\nSJF Performance:\n");
    for(int i=0; i<team->playerCount && ballsLeft > 0; ++i)
    {
        int balls = ( ballsLeft > 30) ? 30: ballsLeft;
        int runs = sjfbatters[i].strikeRate * balls / 100;
        sfjRuns += runs;
        ballsLeft -= balls;
        sjfwicket++;

        printf("%s scored %d runs in %d balls.\n", sjfbatters[i].name, runs, balls);

    }

    printf("\nFinal Score: %d runs \nWickets: %d", sfjRuns, sjfwicket - 1);

    //srtf
    Player* srtfbatters = (Player* )malloc (team->playerCount * sizeof(Player));
    memcpy(srtfbatters, team->players, team->playerCount * sizeof(Player));
    qsort(srtfbatters, team->playerCount, sizeof(Player), compareSJF);


    int srtfRuns = 0;
    ballsLeft = 120;
    int srtfwicket =0;
    printf("\nSRTF Performance:\n");
    for(int i=0; i<team->playerCount && ballsLeft > 0; ++i)
    {
        int balls = ( ballsLeft > 30) ? 30: ballsLeft;
        int runs = srtfbatters[i].strikeRate * balls / 100;
        runs *= (srtfbatters[i].currentForm / 100.0);

        srtfRuns += runs;
        ballsLeft -= balls;
        srtfwicket++;

        printf("%s scored %d runs in %d balls with Form: %f\n", srtfbatters[i].name, runs, balls, srtfbatters[i].currentForm);

    }

    printf("\nFinal Score: %d runs \nWickets: %d", srtfRuns, srtfwicket - 1);

    printf("\nBowling Performance:\n");
    bowlingOrderRR(team);

    report(sfjRuns, srtfRuns);  // generates report

    free(srtfbatters);
    free(sjfbatters);
    return;
}


int main()
{
    Team team;
    printf("Enter number of players: ");
    scanf("%d", &team.playerCount);
    if(team.playerCount < 1 || team.playerCount > 11) 
    {
        printf("Invalid number of players!\n");
        printf("Enter number of players: ");
        scanf("%d", &team.playerCount);
    }

    team.players = (Player*)malloc(team.playerCount * sizeof(Player));  // DMA

    inputData(&team);

    battingOrderSJF(&team);
    battingOrderSRTF(&team);
    bowlingOrderRR(&team);

    match(&team);

    free(team.players);

    return 0;
}