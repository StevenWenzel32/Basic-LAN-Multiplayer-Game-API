// this holds the protocols and code to run the knight/host side of the platformer game Knight and Slime
// these are the portocols to run once the 2 players are connected

// my files
#include "knightHost.hpp"

class KnightHost : public Player{

};



// manage the game world and the host player
int main (int argc, char* argv[]) {
    // create/launch the level
    
    // while loop 
        // continually manage the host/knight players positions and actions
        // contnually check for updates from the client/slime player

    // while loop when they are outside a level - until the host player disconnects

    return 0;
} 
// connect to the server, connect to another player, disconnect from the server
// check for messages from the server 
int main (int argc, char* argv[]) {
    // check that the command line the right # of params
    if (argc < 4){
        cerr << "Error: Not enough parameters passed in. Usage: " << argv[0] << " <PORT>, <Method>, <Host/URL>\n";
        return 1;
    }

    // params passed in through command line
    string port = argv[1];
    string method = argv[2];
    string url = argv[3];

    // parse the input
    pair<string, string> hostParse = parseHost(url);

    // build
    string msg = buildRequest(method, hostParse.first, hostParse.second);

    // make the socket structs and error check
    // pass in serverIP/domain -- getaddrinfo does the DNS lookup for us, how nice!
    struct addrinfo* servinfo = makeGetaddrinfo(hostParse.first, port);

    // make the socket
    int clientSd = makeSocket(servinfo);

    // connect the socket to the server
    connectSocket(clientSd, servinfo);

    // ensure all the data in the msg is sent
    sendAllData(clientSd, msg);

    // recieve the server response
    string reply = readResponse(clientSd);

    // process server response
    processResponse(reply);
    
    // call that handles error checks in other function
    closeSocket(clientSd);

    return 0;
}