#include "serveur.h"

int shmid;
current_auction *currentAuction;
sem_t *sem;

/**
 * function main
 * @brief Main function of the auction manager
 * @return int
 */
int main()
{
    // Initialize the semaphore
    sem = sem_open("/auction_sem", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

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

    displayASCIIAuction();
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

    sleep(5);

    // Cycle through all items (each iteration of this loop is a new round of bidding)
    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        currentAuction->current_bid_item = i;
        currentAuction->last_bidder = -1;
        printf("\n+-------------------------------------+\n");
        printf("|        Auction Round %d              |\n", i + 1);
        printf("+-------------------------------------+\n");
        printf("|  Bidding on item : %-16s |\n", currentAuction->item_list[currentAuction->current_bid_item].name);
        printf("+-------------------------------------+\n");

        // printf("\nBidding on item %s\n", currentAuction->item_list[currentAuction->current_bid_item].name);
        currentAuction->last_bid = currentAuction->item_list[currentAuction->current_bid_item].price;

        for (int j = 0; j < PLAYER_SIZE; j++)
        {
            if (currentAuction->bidders[j].pid != 0) {
                printf("\nSending signal to client %d\n", currentAuction->bidders[j].pid);
                kill(currentAuction->bidders[j].pid, SIGUSR1);
            }
        }

        // Wait for the semaphore
        sem_wait(sem);
        sleep(65);
       
        if (currentAuction->last_bidder == -1)
        {
            printf("\nNo one bid on the item\n");
        } else {
            printf("\n+-------------------------------------+\n");
            printf("|        Auction Result               |\n");
            printf("+-------------------------------------+\n");
            printf("|  Winner : %-25s |\n", currentAuction->bidders[currentAuction->last_bidder].name);
            printf("|  Bid    : %-25d |\n", currentAuction->last_bid);
            printf("+-------------------------------------+\n");
            currentAuction->bidders[currentAuction->last_bidder].money = currentAuction->bidders[currentAuction->last_bidder].money - currentAuction->last_bid;
            currentAuction->item_list[currentAuction->current_bid_item].winner = currentAuction->last_bidder;
        }
        // Post the semaphore
        sem_post(sem);
    }

    printf("\nAuction is over ! \n");

    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        if (currentAuction->bidders[i].pid != 0)
        {
            kill(currentAuction->bidders[i].pid, SIGUSR2);
        }
    }

    // Print the sum of rarity of all items won by each bidder
    printf("+---------------------+---------------------+\n");
    printf("|      Player         |     Total Rarity    |\n");
    printf("+---------------------+---------------------+\n");

    for (int i = 0; i < PLAYER_SIZE; i++) {
        int sum = 0;
        for (int j = 0; j < PLAYER_SIZE; j++) {
            if (currentAuction->item_list[j].winner == i) {
                sum += currentAuction->item_list[j].rarity;
            }
        }
        printf("| %-19s | %-19d |\n", currentAuction->bidders[i].name, sum);
    }

    printf("+---------------------+---------------------+\n");

    sleep(5);

    // Kill all bidders
    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        if (currentAuction->bidders[i].pid != 0)
        {
            kill(currentAuction->bidders[i].pid, SIGKILL);
        }
    }

    shmdt(currentAuction);

    shmctl(shmid, IPC_RMID, NULL);

    // Close the semaphore
    sem_close(sem);

    // Unlink the semaphore
    sem_unlink("/auction_sem");

    return 0;
}

/**
 * function handleConnection
 * @brief Handle the connection of a bidder
 * @return void
 */
void handleConnection()
{
    printf("Received signal from client %d\n", currentAuction->bidders[currentAuction->last_bidder].pid);
    printBidder(currentAuction->last_bidder);
    printItem(currentAuction->last_bidder);

    currentAuction->last_bidder += 1;

    // if all bidders have connected display the bidders
    if (currentAuction->last_bidder == PLAYER_SIZE)
    {
        printBidders();
        printItems();
    }
}

/**
 * function printBidders
 * @brief print all bidders
 * @return void
 * @warning Do not use this function before every bidder has connected
 */
void printBidders()
{
    printf("\n");
    printf("+-------------------------------------------------------------+\n");
    printf("|                         Current Bidders                     |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("|  ID  |         Name         |      PID      |     Money     |\n");
    printf("+-------------------------------------------------------------+\n");
    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        printf("|  %-3d | %-20s | %-13d | %-13d |\n", i, currentAuction->bidders[i].name, currentAuction->bidders[i].pid, currentAuction->bidders[i].money);
    }
    printf("+-------------------------------------------------------------+\n");
    printf("\n");
}

/**
 * function printBidder
 * @brief print a specific bidder
 * @param i The index of the bidder
 * @return void
 */
void printBidder(int i)
{
    printf("\n");
    printf("+-------------------------------------+\n");
    printf("|            Bidder %d                 |\n", i);
    printf("+-------------------------------------+\n");
    printf("|  PID  : %-27d |\n", currentAuction->bidders[i].pid);
    printf("|  Name : %-27s |\n", currentAuction->bidders[i].name);
    printf("|  Money: %-27d |\n", currentAuction->bidders[i].money);
    printf("+-------------------------------------+\n");
    printf("\n");
}

/**
 * function printItems
 * @brief print all items
 * @return void
 * @warning Do not use this function before every item is chosen
 */
void printItems()
{
    printf("\n");
    printf("+-------------------------------------------------------------+\n");
    printf("|                        Current Items                        |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("|         Name          |      Price      |     Rarity        |\n");
    printf("+-------------------------------------------------------------+\n");
    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        printf("| %-21s | %-15d | %-17d |\n", currentAuction->item_list[i].name, currentAuction->item_list[i].price, currentAuction->item_list[i].rarity);
    }
    printf("+-------------------------------------------------------------+\n");
    printf("\n");
}

/**
 * function printItem
 * @brief print a specific item
 * @param i The index of the item
 * @return void
 */
void printItem(int i)
{
    // printf("\tName: %s\n", currentAuction->item_list[i].name);
    // printf("\tPrice: %d\n", currentAuction->item_list[i].price);
    // printf("\tRarity: %d\n", currentAuction->item_list[i].rarity);
    printf("\n");
    printf("+-------------------------------------+\n");
    printf("|            Item %d                   |\n", i);
    printf("+-------------------------------------+\n");
    printf("|  Name : %-27s |\n", currentAuction->item_list[i].name);
    printf("|  Price: %-27d |\n", currentAuction->item_list[i].price);
    printf("|  Rarity: %-25d  |\n", currentAuction->item_list[i].rarity);
    printf("+-------------------------------------+\n");
    printf("\n");
}

/**
 * function displayASCIIAuction
 * @brief Displays an ASCII art of an auction
 * @return void 
 */
void displayASCIIAuction() {
    printf("                         ________\n");
    printf("                        |        |\n");
    printf("                        | Vendu! |\n");
    printf("                        |________|\n");
    printf("                             |\n");
    printf("                             |\n");
    printf("        ______               |\n");
    printf("       |      |              |\n");
    printf("       |  O O |              |\n");
    printf("       |   ^  |              |\n");
    printf("       | (__) |              |\n");
    printf("       |______|              |\n");
    printf("          |\n");
    printf("          |\n");
    printf("         /|\\\n");
    printf("        / | \\\n");
    printf("       /  |  \\\n");
    printf("      /   |   \\\n");
    printf("     /____|____\\\n");
    printf("    |_____|_____|\n");
    printf("   /            \\\n");
    printf("  /              \\\n");
    printf(" /                \\\n");
    printf("/__________________\\\n");
    printf("|__________________|\n");
    printf("         |  |\n");
    printf("         |  |\n");
    printf("         |  |\n");
    printf("         |  |\n");
    printf("         |__|\n");
    printf("\n");

    printf("           ____  ____  ____  ____  ____  ____  ____\n");
    printf("          /    \\/    \\/    \\/    \\/    \\/    \\/    \\\n");
    printf("         |                                          |\n");
    printf("          \\____/\\____/\\____/\\____/\\____/\\____/\\____/\n");
    printf("           ____  ____  ____  ____  ____  ____  ____\n");
    printf("          /    \\/    \\/    \\/    \\/    \\/    \\/    \\\n");
    printf("         |                                          |\n");
    printf("          \\____/\\____/\\____/\\____/\\____/\\____/\\____/\n");
    printf("           ____  ____  ____  ____  ____  ____  ____\n");
    printf("          /    \\/    \\/    \\/    \\/    \\/    \\/    \\\n");
    printf("         |                                          |\n");
    printf("          \\____/\\____/\\____/\\____/\\____/\\____/\\____/\n");
    printf("           ____  ____  ____  ____  ____  ____  ____\n");
    printf("          /    \\/    \\/    \\/    \\/    \\/    \\/    \\\n");
    printf("         |                                          |\n");
    printf("          \\____/\\____/\\____/\\____/\\____/\\____/\\____/\n");
    printf("\n");
}