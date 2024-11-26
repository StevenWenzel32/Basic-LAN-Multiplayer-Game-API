// this holds the protocols and code to run the Slime/Client side of the platformer game: Knight and Slime
// these are the portocols to run once the 2 players are connected
// holds the main for the client side, listens on the broadcast and sends on the broadcast in a loop, until exit

// my files
#include "slimeClient.hpp"

// function with loop to run while in a game

int main (int argc, char* argv[]) {
    // setup the listening socket for broadcast msgs
    struct addrinfo* clientinfo = makeAddrinfo("udp", PORT);
    // make the socket
    this.broadSd = makeSocket(clientinfo);
    // set the option to resuse 
    setSocketReuse(this.broadSd);
    // turn on the broadcast - for sending 
    setSocketBroadcast(this.broadSd);
    // bind the socket - for listening 
    bindSocket(this.broadSd, clientinfo);
    // free after being used -- will probably be moved again 
    freeaddrinfo(clientinfo);

    // setup the socket for sending broadcast msgs
    // use ipv4
    this.broadcastAddr.sin_family = AF_INET;
    // convert the port to network byte order
    this.broadcastAddr.sin_port = htons(atoi(PORT));
    // set the boradcast addr
    this.broadcast.sin_addr.s_addr = inet_addr("255.255.255.255");

    // Thread for listening for broadcast msgs -- always running until shutdown
    // must make a listenForMsgs() *************
    thread listenerThread(listenForMsgs, this.broadSd);

    // main thread sends msgs
    sendMessages(this.broadSd, clientinfo);
    
    // make sure the listeing thread has ended before closing
    listenerThread.join();
    
    // close the broadcast socket used for listening and sending
    closeSocket(this.broadSd);
    return 0;
} 

// listen for msgs on the broadcast
void listenForMsgs(){
    while (true){
        // process msgs in new threads
        processMsgs();
            
        // send out any msgs the user wants to send 
    }
}