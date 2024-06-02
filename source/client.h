/*******************************************/
/*		I N C L U D E S                    */
/*******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

/*******************************************/
/*		G L O B A L S                      */
/*******************************************/

#define SHM_KEY 8000
#define PLAYER_SIZE 3

/*******************************************/
/*		S T R U C T U R E S                */
/*******************************************/

typedef struct
{
    char name[20];
    int rarity;
    int price;
    int winner;
} item;

typedef struct 
{
    int pid;
    char name[20];
    int money;
} bidder;

typedef struct 
{
    int last_bid;
    int last_bidder;
    int current_bid_item;
    bidder bidders[PLAYER_SIZE];
    int server_pid;
    item item_list[PLAYER_SIZE];
} current_auction;

/*******************************************/
/*		P R O T O T Y P E S                */
/*******************************************/
void connectToServer();
void chooseItem();
void bidInstance();
void displayEndResults();