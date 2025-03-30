// M Kaleem Akhtar
// 23i-0524
// CS-C


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include<stdbool.h>

#define maxPlayers 4
#define maxZombies 5
#define mapSize 10

typedef enum
{
    MOVE,
    ATTACK,
    HIDE
}ActionType;

typedef enum
{
    PLAYER,
    ZOMBIE
}EntityType;


// for message communication
typedef struct
{
    int id;
    int x,y;
    EntityType type;
    ActionType action;
    int health;
}GameMessage;


// rollno to determine zombies
int zombieCount()
{
    int last = 0524 % 10000;
    return (last * 5) % 20;
}

void playerProcess(int id, int po)
{
    GameMessage msg = {id, PLAYER, MOVE, 0,0, 100};

    while(1)
    {
        int action = rand() % 10;
        if(action < 6){msg.action = MOVE;}
        else if (action < 9){msg.action = ATTACK;}
        else msg.action = HIDE;

        msg.x = rand() % 3 - 1;
        msg.y = rand() % 3 - 1;

        write(po, &msg, sizeof(GameMessage));
        sleep(1);
    }
}

void zombieProcess(int id, int target, int po)
{
    GameMessage msg = {id, ZOMBIE, MOVE, 0,0,10};

    while(1)
    {
        int action = rand() % 10;
        if(action < 8){msg.action = MOVE;}
        else msg.action = ATTACK;
        
        msg.x = rand() % 3 - 1;
        msg.y = rand() % 3 - 1;

        write(po, &msg, sizeof(GameMessage));
        sleep(1);
    }
}



int main()
{
    srand(time(NULL));
    
    int noPlayers = maxPlayers;
    int noZombies = maxZombies;

    printf("\nHomo-Sapiens VS Zombies\n");

    int playerPipe[maxPlayers][2];
    int zombiePipe[maxZombies][2];

    //player processes
    for(int i=0; i<noPlayers; ++i)
    {
        pipe(playerPipe[i]);
        if(fork() == 0)
        {
            close(playerPipe[i][0]);
            playerProcess(i, playerPipe[i][1]);
            exit(0);
        }
        close(playerPipe[i][1]);
    }


    //zombie processes
    for(int i=0; i<noZombies; ++i)
    {
        pipe(zombiePipe[i]);
        if(fork() == 0)
        {
            close(zombiePipe[i][0]);
            zombieProcess(i, rand() % noPlayers, zombiePipe[i][1]);
            exit(0);
        }
        close(zombiePipe[i][1]);
    }
    int deadPlayer =0;
    int deadZombie =0;
    bool gameOver = false;

    int playerHealth[maxPlayers];
    int zombieHealth[maxZombies];
    for(int i=0; i<noPlayers; ++i)
    {
        playerHealth[i] = 100;
    }
    for (int i = 0; i < noZombies; i++)
    {
        zombieHealth[i] = 10;
    }
    
    while(!gameOver)
    {
        for(int i=0; i<noPlayers; ++i)
        {
            GameMessage msg;
            if(playerHealth[i] <=0) continue;

            if(read(playerPipe[i][0], &msg, sizeof(GameMessage) > 0))
            {
                printf("\nPlayer %d: ", msg.id);
                if(msg.action == MOVE)
                {
                    printf("\nMoved to Position(%d,%d)",msg.x, msg.y);
                }
                else if(msg.action == ATTACK)
                {
                    printf("\nPlayer Attacked Zombie.");
                    int target = rand()% noZombies;
                    zombieHealth[i] -= 10;
                    printf("\nZombie %d died.",target);
                    ++deadZombie;
                }
                else if(msg.action == HIDE)
                {
                    printf("\nPlayer is Hiding.");
                }
            }
        }
        for(int i=0; i<noZombies; ++i)
        {
            GameMessage msg;
            if(zombieHealth[i] <=0) continue;

            if(read(zombiePipe[i][0], &msg, sizeof(GameMessage) > 0))
            {
                printf("\nZombie %d: ",msg.id);
            }
            if(msg.action == MOVE)
            {
                printf("\nZombie Moved toward Player.");
            }
            else
            {
                printf("\nZombie Attacked a player.");
                if(noPlayers >0)
                {
                    int target = rand() % noPlayers;
                    playerHealth[target] -= 50;
                    if(playerHealth[target] <= 0)
                    {
                        deadPlayer++;
                        printf("\nPlayer %d Died.", target);
                    }
                }

            }
        }

        if(deadPlayer == noPlayers)
        {
            gameOver = true;
            printf("\n\nZombies Win! All players are dead!\n");
        }
        if(deadZombie == noZombies)
        {
            gameOver = true;
            printf("\n\nHomo-Sapiens Win! All zombies are dead!\n");
        }
        sleep(1);
    }

    // end child processes
    for(int i=0; i<noPlayers; ++i)
    {
        wait(NULL);
    }
    for(int j=0; j<noZombies; ++j)
    {
        wait(NULL);
    }
    printf("\n Result: Players alive %d/%d, Zombies alive %d/%d\n", noPlayers - deadPlayer, noPlayers, noZombies - deadZombie, noZombies);
    printf("\nGAME ENDS\n");
    return 0;
}