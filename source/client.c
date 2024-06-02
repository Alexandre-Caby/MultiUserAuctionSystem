#include "client.h"

int shmid;
int bidder_id;
current_auction *currentAuction;

int main()
{
    // Signal handler
    signal(SIGUSR1, bidInstance);

    connectToServer();

    printf("Connected to auction, server pid: %d\n", currentAuction->server_pid);

    // Notify the server that the client is ready
    kill(currentAuction->server_pid, SIGUSR1);

    while (1);
}

/**
 * Request the user to choose an item name
 */
void chooseItem()
{
    printf("Choose a name for your item\n");
    char item[20];
    scanf("%s", item);
    while (strlen(item) > 20)
    {
        printf("Name too long. Please enter a name with less than 20 characters: ");
        scanf("%s", item);
    }

    int rarity = rand() % 4;
    int price = rand() % 400 + 100;

    printf("Your item (%s) is of rarity %d ! Its starting price is %d\n", item, rarity, price);

    currentAuction->item_list[bidder_id].price = price;
    strcpy(currentAuction->item_list[bidder_id].name, item);
    currentAuction->item_list[bidder_id].rarity = rarity;
}

/**
 * Connect to the server and attach to the shared memory
 */
void connectToServer()
{
    char name[20];
    printf("Enter your name: ");
    scanf("%s", name);
    while (strlen(name) > 20)
    {
        printf("Name too long. Please enter a name with less than 20 characters: ");
        scanf("%s", name);
    }

    shmid = shmget(SHM_KEY, 1024, 0666);
    currentAuction = (current_auction *)shmat(shmid, (void *)0, 0);

    bidder b;
    b.pid = getpid();
    strcpy(b.name, name);
    srand(time(NULL));
    b.money = rand() % 100 + 900;

    bidder_id = currentAuction->last_bidder;

    chooseItem();

    currentAuction->bidders[currentAuction->last_bidder].pid = b.pid;
    strcpy(currentAuction->bidders[currentAuction->last_bidder].name, b.name);
    currentAuction->bidders[currentAuction->last_bidder].money = b.money;
}

void bidInstance()
{
    system("clear");

    printf("Current item: %s\n", currentAuction->item_list[currentAuction->current_bid_item].name);
    printf("Current bid: %d\n", currentAuction->last_bid);
    // printf("Current bidder: %d\n", currentAuction->bidders[currentAuction->last_bidder].name);
    printf("Your money: %d\n", currentAuction->bidders[bidder_id].money);

    if (currentAuction->bidders[bidder_id].money > currentAuction->last_bid)
    {
        printf("Enter your bid if you want to bid: ");
        int bid;
        scanf("%d", &bid);
        if (bid > currentAuction->last_bid && bid <= currentAuction->bidders[bidder_id].money)
        {
            currentAuction->last_bid = bid;
            currentAuction->last_bidder = bidder_id;
            printf("Bid successful\n");
            sleep(1);
            for (int i = 0; i < PLAYER_SIZE; i++)
            {
                if(currentAuction->bidders[i].pid != 0 && i != bidder_id)
                kill(currentAuction->bidders[i].pid, SIGUSR1);
            }
        }
        else
        {
            printf("Bid too low or you don't have enough money\n");
            sleep(1);
            bidInstance();
        }
    }
    else
    {
        printf("You don't have enough money to bid.. Waiting for the current round to end...\n");
    }
}
