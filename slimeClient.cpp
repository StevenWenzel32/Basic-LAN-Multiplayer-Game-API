// this holds the protocols and code to run the Slime/Client side of the platformer game: Knight and Slime
// these are the portocols to run once the 2 players are connected
// holds the main for the client side, listens on the broadcast and sends on the broadcast in a loop, until exit

// my files
#include "slimeClient.hpp"

// function with loop to run while in a game

int main (int argc, char* argv[]) {
    // setup the listening socket for boradcast msgs
    struct addrinfo* clientinfo = makeAddrinfo("udp", PORT);
    // make the socket
    int clientListenSd = makeSocket(clientinfo);
    // set the option to resuse 
    setSocketReuse(clientListenSd);
    // bind the socket
    bindSocket(clientListenSd, clientinfo);
    
    // setup the socket for sending broadcast msgs
    int clientSendSd = makeSocket(clientinfo);
    setSocketReuse(clientListenSd);

    // free after being used -- will porbably be moved again 
    freeaddrinfo(clientinfo);

    // Thread for listening for broadcast msgs -- always running until shutdown
    // must make a listenForMsgs() *************
    thread listenerThread(listenForMsgs, clientListenSd);

    // main thread sends msgs
    sendMessages(clientSendSd, clientinfo);
    
    // make sure the listeing thread has ended before closing
    listenerThread.join();
    
    // close broadcast listening
    closeSocket(clientListenSd);
    // close broadcast sending
    closeSocket(clientSendSd);
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