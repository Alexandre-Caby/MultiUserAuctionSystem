#include "serveur.h"

int main()
{
    current_auction auction;
    auction.price = 0;
    auction.last_bid = 0;
    auction.last_bidder = 0;
    auction.server_pid = getpid();
    for (int i = 0; i < 3; i++)
    {
        auction.bidders[i].pid = 0;
    }

    printf("Welcome to the auction system\n");

    int shmid = shmget(SHM_KEY, 1024, 0666 | IPC_CREAT);
    struct current_auction *shmptr = (struct current_auction *)shmat(shmid, (void *)0, 0);

    memcpy(shmptr, &auction, sizeof(auction));

    printf("Server pid: %d\n", auction.server_pid);
    while(auction.bidders[2].pid == 0)
    {
       signal(SIGUSR1, handleConnection);
    }
    return 0;
}

void handleConnection()
{
    current_auction auction = getAuction();
    printf("Received signal from client %d\n", auction.bidders[auction.last_bidder].pid);
    auction.last_bid = auction.bidders[auction.last_bidder].money;
    auction.price = auction.last_bid;
    auction.last_bidder = (auction.last_bidder + 1) % 3;

    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    struct current_auction *shmptr = (struct current_auction *)shmat(shmid, (void *)0, 0);
    memcpy(shmptr, &auction, sizeof(auction));
}

current_auction getAuction()
{
    int shmid = shmget(SHM_KEY, 1024, 0666);
    struct current_auction *shmptr = (struct current_auction *)shmat(shmid, (void *)0, 0);
    current_auction auction;

    if (shmptr != NULL)
    {
        memcpy(&auction, shmptr, sizeof(auction));
    }
    else
    {
        auction.server_pid = -1;
    }

    return auction;
}
