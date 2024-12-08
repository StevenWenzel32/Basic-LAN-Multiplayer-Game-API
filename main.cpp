// my files
// the game related and specifc protocols - this file should include the basicNetworking
#include "player.hpp"
#include "globalFlags.hpp"

#include <signal.h>      // for the shutdown signal
#include <csignal>

// port to use to listen and send on broadcast
#define UDP_PORT "2087"
#define TCP_PORT "2088"
// # of connection requests for the server side of the players to be listening for 
#define BACKLOG 1

int main (int argc, char* argv[]) {
    // Set up signal handling for SIGINT and SIGTERM so that the client can shutdown nicely
    signal(SIGINT, GlobalFlags::signalHandler);
    signal(SIGTERM, GlobalFlags::signalHandler);
    signal(SIGTSTP, GlobalFlags::signalHandler);

    // create a player - once the player starts a game a game object will be created
    Player player;

    // setup the listening socket for broadcast msgs
    struct addrinfo* broadinfo = makeAddrinfo("udp", UDP_PORT);
    // make the socket
    player.broadSd = makeSocket(broadinfo);
    // set the option to resuse 
    setSocketReuse(player.broadSd);
    setNonblocking(player.broadSd);
    // turn on the broadcast - for sending 
    setSocketBroadcast(player.broadSd);
    // bind the socket - for listening 
    bindSocket(player.broadSd, broadinfo);
    // free after being used -- will probably be moved again 
    freeaddrinfo(broadinfo);
    // setup the same socket as above for sending broadcast msgs
    // use ipv4
    player.broadcastAddr.sin_family = AF_INET;
    // convert the port to network byte order
    player.broadcastAddr.sin_port = htons(atoi(UDP_PORT));
    // set the boradcast addr - old
//    player.broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    // new
    string broadcastAddr = getBroadcastAddr();
    if (!broadcastAddr.empty()){
        player.broadcastAddr.sin_addr.s_addr = inet_addr(broadcastAddr.c_str());
    } else {
        cerr << "unable to determine the boradcast address" << endl;
    }

    // setup the TCP listening socket for connecting to other players
    // get the info 
    struct addrinfo* tcpinfo = makeAddrinfo("tcp", TCP_PORT);
    player.tcpListenSd = makeSocket(tcpinfo);
//cout << "TCP listen SD = " << player.tcpListenSd << endl;
    setSocketReuse(player.tcpListenSd);
    setNonblocking(player.tcpListenSd);
    bindSocket(player.tcpListenSd, tcpinfo);
    listening(player.tcpListenSd, BACKLOG);
//cout << "socket setup done" << endl;

    // Thread for listening for broadcast msgs -- non blocking -- always running until shutdown
    thread udpListenThread(&Player::listenForUdpMsgs, &player);
//cout << "now listening on broadcast on = thread 1" << endl;
    // thread for listening to tcp msgs -- non blocking -- always running until shutdown
    thread tcpListenThread(&Player::listenForTcpConnect, &player);
//cout << "now listening for TCP connections from players on = thread 2" << endl;

//cout << "can now send msgs on the main thread = thread 3" << endl;
    // loop until 
    // // main thread - handles the program cmds most end up calling a send msg func -- starts the game thread 
    player.inputPrompt();
//cout << "main thread is now done sending msgs" << endl;
    
    // make sure the listening threads have ended before closing
    udpListenThread.join();
    tcpListenThread.join();

    // once shutting down join all the threads
    for (thread* thread : player.threads) {
        if (thread->joinable()){
            thread->join();
        }
        delete thread;
    }
    player.threads.clear();

//cout << "closing sockets in main" << endl;
    // close the broadcast socket used for listening and sending
    closeSocket(player.broadSd);
    closeSocket(player.tcpListenSd);
    return 0;
} 