// this holds the protocols and code to run the knight/host side of the platformer game Knight and Slime
// these are the portocols to run once the 2 players are connected

// go through and get rid of the includes that aren't needed
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <string.h>       // memset
#include <errno.h>        // errno
#include <fstream>        // ofstream for file creation
#include <netdb.h>        // gethostbyname
#include <sstream>        // for stringstream stuff
#include <random>         // for random #
#include <chrono>         // for steady_clock and timer stuff
#include <queue>          // for queue, duh
#include <cerrno>
#include <cstring>        // for strerror
#include <vector>
#include <array>

using namespace std;

// manage the game world and the host player
int main (int argc, char* argv[]) {
    // create/launch the level
    
    // while loop 
        // continually manage the host/knight players positions and actions
        // contnually check for updates from the client/slime player

    // while loop when they are outside a level - until the host player disconnects

    return 0;
} 