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
    int can_bid = 1;

        if (currentAuction->last_bid == currentAuction->item_list[currentAuction->current_bid_item].price)
        {
            switch (currentAuction->current_bid_item)
            {
            case 0:
                system("clear");
                printf("Auction is starting.. Bidding on first item\n");
                sleep(5);
                break;
            case 1:
                system("clear");
                printf("Auction is starting.. Bidding on second item\n");
                sleep(5);
                break;
            case 2:
                system("clear");
                printf("Auction is starting.. Bidding on third item\n");
                sleep(5);
                break;
            default:
                break;
            }
        }
        else
        {
            system("clear");
            printf("New bid detected!\n");
            sleep(1);
        }

    while (can_bid)
    {

        system("clear");

        // print the current item bid
        printf("Current item: %s\n", currentAuction->item_list[currentAuction->current_bid_item].name);
        printf("Current bid: %d\n", currentAuction->last_bid);

        if (currentAuction->last_bidder != -1)
            printf("Last bidder: %s\n", currentAuction->bidders[currentAuction->last_bidder].name);

        printf("Your money: %d\n", currentAuction->bidders[bidder_id].money);

        if (currentAuction->bidders[bidder_id].money > currentAuction->last_bid)
        {
            int bid;
            printf("Enter your bid: ");
            scanf("%d", &bid);
            if( bid == 0) {
                printf("Skipping bidding on this item\n");
                can_bid = 0;
            }
            else if (bid > currentAuction->last_bid && bid <= currentAuction->bidders[bidder_id].money)
            {
                currentAuction->last_bid = bid;
                currentAuction->last_bidder = bidder_id;
                printf("Bid successful\n");
                sleep(0.2);
                for (int i = 0; i < PLAYER_SIZE; i++)
                {
                    if (currentAuction->bidders[i].pid != 0 && i != bidder_id)
                        kill(currentAuction->bidders[i].pid, SIGUSR1);
                }
                can_bid = 0;
            }
            else
            {
                printf("Bid too low or you don't have enough money\n");
                sleep(1);
            }
        }
        else
        {
            printf("You don't have enough money to bid.. Waiting for the current round to end...\n");
            can_bid = 0;
        }
    }
    sem_post(sem);
}

/**
 * function displayEndResults
 * @brief Displays the results of the auction
 * @return void 
 */
void displayEndResults()
{
    system("clear");
    printf("Auction is over ! \n");

    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        int sum = 0;
        for (int j = 0; j < PLAYER_SIZE; j++)
        {
            if (currentAuction->item_list[j].winner == i)
            {
                sum += currentAuction->item_list[j].rarity;
            }
        }
        if (sum == 0)
        {
            printf("%s didn't win any items\n", currentAuction->bidders[i].name);
        }
        else
        {
            printf("%s won items with a total rarity of %d\n", currentAuction->bidders[i].name, sum);
        }
    }

    sleep(10);
    auctionActive = 0;
}
