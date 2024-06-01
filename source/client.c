#include "client.h"

int main()
{
    char name[20];
    printf("Enter your name: ");
    scanf("%s", name);
    while (strlen(name) > 20)
    {
        printf("Name too long. Please enter a name with less than 20 characters: ");
        scanf("%s", name);
    }

    current_auction auction = getAuction();

    if (auction.server_pid == -1)
    {
        printf("No auction is currently running.\n");
        return 0;
    }

    // Connect to the auction
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    struct current_auction *shmptr = (struct current_auction *)shmat(shmid, (void *)0, 0);

    bidder b;
    b.pid = getpid();
    strcpy(b.name, name);
    b.money = rand() % 100 + 900;

    memcpy(&auction.bidders[auction.last_bidder], &b, sizeof(b));

    memcpy(shmptr, &auction, sizeof(auction));

    //on log le pid du serveur
    printf("Connected to auction, server pid: %d\n", auction.server_pid);

    kill(auction.server_pid, SIGUSR1);

    sleep(60);
}

current_auction getAuction()
{
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, 1024, 0666);
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