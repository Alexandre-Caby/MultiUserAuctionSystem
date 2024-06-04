#include "client.h"

int shmid;
int bidder_id;
current_auction *currentAuction;
int auctionActive = 1;
sem_t *sem;

/**
 * function main
 * @brief Main function of the client program
 * @return int 
 */
int main()
{
    struct sigaction sa;

    // Signal handler
    sa.sa_handler = bidInstance;
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = displayEndResults;
    sigaction(SIGUSR2, &sa, NULL);

    connectToServer();

    printf("Connected to auction, server pid: %d\n", currentAuction->server_pid);

    // Notify the server that the client is ready
    kill(currentAuction->server_pid, SIGUSR1);

    while(auctionActive);

    return 0;
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

    int rarity = rand() % 4 + 1;
    int price = rand() % 400 + 100;

    printf("Your item (%s) is of rarity %d ! Its starting price is %d\n", item, rarity, price);

    bidder_id = currentAuction->last_bidder;

    currentAuction->item_list[bidder_id].price = price;
    strcpy(currentAuction->item_list[bidder_id].name, item);
    currentAuction->item_list[bidder_id].rarity = rarity;
}

/**
 * function connectToServer
 * @brief Connects the client to the server
 * @return void 
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
    
    sem = sem_open("/auction_sem", 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    bidder b;
    b.pid = getpid();
    strcpy(b.name, name);
    srand(time(NULL));
    b.money = rand() % 100 + 900;

    chooseItem();

    currentAuction->bidders[currentAuction->last_bidder].pid = b.pid;
    strcpy(currentAuction->bidders[currentAuction->last_bidder].name, b.name);
    currentAuction->bidders[currentAuction->last_bidder].money = b.money;
}

/**
 * function bidInstance
 * @brief Function that handles the bidding process
 * @return void 
 */

void bidInstance()
{
    bool can_bid = true;
    bool skip_bid = false; 

    if (currentAuction->last_bid == currentAuction->item_list[currentAuction->current_bid_item].price)
    {
        system("clear");
        switch (currentAuction->current_bid_item)
        {
            case 0:
                printf("Auction is starting.. Bidding on first item\n");
                break;
            case 1:
                printf("Auction is starting.. Bidding on second item\n");
                break;
            case 2:
                printf("Auction is starting.. Bidding on third item\n");
                break;
            default:
                printf("Auction is starting.. Bidding on another item\n");
                break;
        }
        sleep(5);
    }
    else
    {
        system("clear");
        printf("New bid detected!\n");
        sleep(1);
    }

    while (can_bid && !skip_bid)
    {
        system("clear");

        printf("+--------------------------------------------+\n");
        printf("|              Current Auction               |\n");
        printf("+--------------------------------------------+\n");
        printf("| Current item: %-28s |\n", currentAuction->item_list[currentAuction->current_bid_item].name);
        printf("| Current bid : %-28d |\n", currentAuction->last_bid);

        if (currentAuction->last_bidder != -1)
            printf("| Last bidder : %-28s |\n", currentAuction->bidders[currentAuction->last_bidder].name);

        printf("| Your money  : %-28d |\n", currentAuction->bidders[bidder_id].money);
        printf("+--------------------------------------------+\n");

        if (currentAuction->bidders[bidder_id].money > currentAuction->last_bid)
        {
            int bid;
            printf("Enter your bid (or enter -1 to skip this item) : ");

            if (!skip_bid) { 
                scanf("%d", &bid);

                if (bid == -1) {
                    printf("You have skipped this item\n");
                    skip_bid = true; 
                } else if (bid > currentAuction->last_bid && bid <= currentAuction->bidders[bidder_id].money) {
                    currentAuction->last_bid = bid;
                    currentAuction->last_bidder = bidder_id;
                    printf("Bid successful\n");
                    sleep(1);

                    for (int i = 0; i < PLAYER_SIZE; i++)
                    {
                        if (currentAuction->bidders[i].pid != 0 && i != bidder_id)
                            kill(currentAuction->bidders[i].pid, SIGUSR1);
                    }
                    can_bid = false;
                    while (1);
                } else if ((bid <= currentAuction->last_bid || bid > currentAuction->bidders[bidder_id].money)) {
                    printf("Bid too low or you don't have enough money\n");
                    sleep(1);
                }
            } else {
                printf("You have skipped your turn\n");
                sleep(1);
                can_bid = false; // Sortez de la boucle de saisie
            }
        }
        else
        {
            printf("You don't have enough money to bid.. Waiting for the current round to end...\n");
            can_bid = false;
        }
    }
    sem_post(sem);
}

/**
 * function displayEndResults
 * @brief Displays the results of the auction
 * @return void 
 */
void displayEndResults() {
    system("clear");
    printf("Auction is over ! \n");

    printf("+-----------------+----------------------+\n");
    printf("|     Player      |      Total Rarity    |\n");
    printf("+-----------------+----------------------+\n");

    for (int i = 0; i < PLAYER_SIZE; i++) {
        int sum = 0;
        for (int j = 0; j < PLAYER_SIZE; j++) {
            if (currentAuction->item_list[j].winner == i) {
                sum += currentAuction->item_list[j].rarity;
            }
        }
        if (sum == 0) {
            printf("| %-15s | %-19s |\n", currentAuction->bidders[i].name, "Didn't win any items");
        } else {
            printf("| %-15s | %-20d |\n", currentAuction->bidders[i].name, sum);
        }
    }

    printf("+-----------------+----------------------+\n");

    sleep(10);
    auctionActive = 0;
}
