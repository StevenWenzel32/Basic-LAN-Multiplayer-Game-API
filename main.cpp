// my files
// the game related and specifc protocols - this file should include the basicNetworking
#include "player.hpp"

#include <signal.h>      // for the shutdown signal
#include <csignal>

// port to use to listen and send on broadcast
#define PORT "2087"

int main (int argc, char* argv[]) {
    // Set up signal handling for SIGINT and SIGTERM so that the client can stop listening nicely
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // create a player - once the player starts a game a game object will be created
    Player player;

    // setup the listening socket for broadcast msgs
    struct addrinfo* clientinfo = makeAddrinfo("udp", PORT);
    // make the socket
    player.broadSd = makeSocket(clientinfo);
    // set the option to resuse 
    setSocketReuse(player.broadSd);
    // turn on the broadcast - for sending 
    setSocketBroadcast(player.broadSd);
    // bind the socket - for listening 
    bindSocket(player.broadSd, clientinfo);
    // free after being used -- will probably be moved again 
    freeaddrinfo(clientinfo);

    // setup the socket for sending broadcast msgs
    // use ipv4
    player.broadcastAddr.sin_family = AF_INET;
    // convert the port to network byte order
    player.broadcastAddr.sin_port = htons(atoi(PORT));
    // set the boradcast addr
    player.broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    // Thread for listening for broadcast msgs -- always running until shutdown
    thread listenerThread(&Player::listenForMsgs, &player);

    // main thread sends msgs - handles the player executing/sending out their broadcast msgs and protocols
    player.sendMsgs(player.broadSd, clientinfo);
    
    // make sure the listening thread has ended before closing
    listenerThread.join();
    
    // close the broadcast socket used for listening and sending
    closeSocket(player.broadSd);
    return 0;
} 