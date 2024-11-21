// this holds the protocols and code to run the Slime/Client side of the platformer game: Knight and Slime
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

// take the game world updates from the host and send client postion and actions to the host
int main (int argc, char* argv[]) {
    // join the level

    // while loop 
        // continually manage the clinet/slime players positions and actions
        // contnually check for updates from the host/knight player

    // while loop when they are outside a level - until the host player disconnects or the client disconnects

    return 0;
} 