#include "serveur.h"

int shmid;
current_auction *currentAuction;

int main()
{
    // Signal handler
    signal(SIGUSR1, handleConnection);

    // Create a blank auction
    current_auction auction;
    auction.last_bid = 0;
    auction.last_bidder = 0;
    auction.server_pid = getpid();
    auction.current_bid_item = -1;
    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        auction.bidders[i].pid = 0;
    }

    printf("Welcome to the auction system\n");

    // Create shared memory
    shmid = shmget(SHM_KEY, 1024, 0666 | IPC_CREAT);
    currentAuction = (current_auction *)shmat(shmid, (void *)0, 0);

    memcpy(currentAuction, &auction, sizeof(auction));

    printf("Waiting for clients to connect...\n");

    // Wait for all bidders to connect : given the bidders are connected in order, we can wait for the last bidder to connect
    while (currentAuction->bidders[PLAYER_SIZE - 1].pid == 0)
        ;

    printf("Auction is starting ! \n");

    // Cycle through all items (each iteration of this loop is a new round of bidding)
    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        currentAuction->current_bid_item = i;
        printf("Bidding on item %s\n", currentAuction->item_list[currentAuction->current_bid_item].name);
        printItems();
        currentAuction->last_bid = currentAuction->item_list[currentAuction->current_bid_item].price;

        for (int j = 0; j < PLAYER_SIZE; j++)
        {
            if (currentAuction->bidders[j].pid != 0)
                kill(currentAuction->bidders[j].pid, SIGUSR1);
        }

        sleep(20);
        currentAuction->bidders[currentAuction->last_bidder].money -= currentAuction->last_bid;
        currentAuction->bidders[currentAuction->last_bidder].item_inventory[currentAuction->current_bid_item] = currentAuction->item_list[currentAuction->current_bid_item];
    }

    printf("Auction is over ! \n");

    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        for (int j = 0; j < PLAYER_SIZE; j++)
        {
            if (currentAuction->bidders[j].item_inventory[i].price == currentAuction->item_list[i].price)
            {
                printf("Item %s was won by %s\n", currentAuction->item_list[i].name, currentAuction->bidders[j].name);
            }
        }
    }

    while (1)
        ;
    return 0;
}

void handleConnection()
{
    printf("Received signal from client %d\n", currentAuction->bidders[currentAuction->last_bidder].pid);
    printBidder(currentAuction->last_bidder);
    printItem(currentAuction->last_bidder);

    currentAuction->last_bidder += 1;
}

/**
 * Print all bidders
 * @description Print all bidders in the current auction
 * @warning Do not use this function before every bidder has connected
 */
void printBidders()
{
    printf("\nCurrent bidders:\n");
    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        printf("Bidder %d: %d\n", i, currentAuction->bidders[i].pid);
        printf("\tName: %s\n", currentAuction->bidders[i].name);
        printf("\tMoney: %d\n", currentAuction->bidders[i].money);
    }
    printf("\n");
}

/**
 * Print a specific bidder
 * @param i The index of the bidder
 */
void printBidder(int i)
{
    printf("Bidder %d: %d\n", i, currentAuction->bidders[i].pid);
    printf("\tName: %s\n", currentAuction->bidders[i].name);
    printf("\tMoney: %d\n", currentAuction->bidders[i].money);
}

/**
 * Print all items
 * @description Print all items in the current auction
 * @warning Do not use this function before every item is chosen
 */
void printItems()
{
    printf("Current items:\n");
    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        printf("\tName: %s\n", currentAuction->item_list[i].name);
        printf("\tPrice: %d\n", currentAuction->item_list[i].price);
        printf("\tRarity: %d\n", currentAuction->item_list[i].rarity);
    }
    printf("\n");
}

/**
 * Print a specific item
 * @param i The index of the item
 */
void printItem(int i)
{
    printf("\tName: %s\n", currentAuction->item_list[i].name);
    printf("\tPrice: %d\n", currentAuction->item_list[i].price);
    printf("\tRarity: %d\n", currentAuction->item_list[i].rarity);
}
