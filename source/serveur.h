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

// variables globales
/*******************************************/
/*		G L O B A L S                      */
/*******************************************/

#define SHM_KEY 8000

/*******************************************/
/*		S T R U C T U R E S                */
/*******************************************/

typedef struct 
{
    int pid;
    char name[20];
    int money;
} bidder;

typedef struct 
{
    int price;
    int last_bid;
    int last_bidder;
    bidder bidders[3];
    int server_pid;
} current_auction;

/*******************************************/
/*		P R O T O T Y P E S                */
/*******************************************/

current_auction getAuction();
void handleConnection();