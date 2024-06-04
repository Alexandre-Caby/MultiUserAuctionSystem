# Auction System
Welcome to the Auction System application! This guide will help you set up and run the Auction system on your local machine.

## Prerequisites
Before you begin, make sure you have the following installed on your system:

- GCC (GNU Compiler Collection)
- Make
These tools are essential for building and running the application.

## Getting Started
Follow these steps to get your Auction System up and running:

1. Cloning the Repository
Clone the Auction System repository to your local machine. You can do this by running the following command in your terminal:

```bash
git clone https://github.com/Alexandre-Caby/MultiUserAuctionSystem
```

2. Navigating to the Project Directory
Once the repository is cloned, navigate to the MultiUserAuctionSystem directory by running:

```bash
cd Auction_System
```

3. Building the Application
Inside the Auction_System directory, you can build the application by running:

```bash
make
```

This command compiles the server and client applications and places the executables in the app directory.

4. Starting the Auction Manager
To start the auction manager, run the following command:

```bash
./app/serveur
```

This will initialize the auction manager, which will begin listening for incoming client connections.

5. Running Client Instances
Open a new terminal window for each client to run (3 max). From the Auction_System directory, start each client by running:

```bash
./app/client
```

Each client will connect to the auction manager, and you can begin participating in auctions.

## Usage
Once the client is running, you will be able to interact with the Auction System through a series of round-based auctions. The client will display the current auction status, and you can place bids on items as they are auctioned.

## Stopping the application
When you have finished all auctions, the auction manager and clients will be stopped automatically. If you need to stop the server manually, you can do so by pressing `Ctrl+C` in the terminal where the auction manager is running.

## Troubleshooting
If you encounter any issues while running the Auction System, make sure that:

The auction manager is running before you try to connect a client.
- You are running the latest version of the repository code.

# Authors
This project was developed by:
- [Augustin Moriceau]([text](https://github.com/Nelyra))
- [Alexandre Caby]([text](https://github.com/Alexandre-Caby))
