// this hold the specifc protocols to run the 2 player platformer Knight and Slime
// these are the portocols to run once the game is made and the 2 players are connected

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

// send get request to a web server
int main (int argc, char* argv[]) {
    // check that the command line has the right # of params
    if (argc < 3){
        cerr << "Error: Not enough parameters passed in. Usage: " << argv[0] << " <testNumber> <windowSize>\n";
        return 1;
    }

    // params passed in through command line
    int testNum = atoi(argv[1]);
    int windowSize = atoi(argv[2]);

    // make the socket structs and error check
    // pass in serverIP/domain -- getaddrinfo does the DNS lookup for us, how nice!
    struct addrinfo* servinfo = makeGetaddrinfo(HOST, PORT);

    // make the socket
    int clientSd = makeSocket(servinfo);

    int resend;
    // if sliding window print out the window size in results 
    if (testNum == 3){
        // get the start time of the test in usec
        int start = startTimer();
        // call the right test, tests call msg builds and they send the msg themselves
        // tests also call readResponse() and processResponse()
        resend = pickTest(testNum, clientSd, servinfo, windowSize);
        // get the run time in usec
        int runTime = stopTimer(start);
        printWindowSizeTests(windowSize, resend, runTime);
    } else {
        resend = pickTest(testNum, clientSd, servinfo, DEFAULT_WINDOW_SIZE);
        printResults(testNum, resend);
    }

    // call that handles error checks in other function
    closeSocket(clientSd);
    freeaddrinfo(servinfo);

    return 0;
} 