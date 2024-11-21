// This is basically the lobby -- handles the player connections

// go through and get rid of the includes that aren't needed
#include <iostream>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close, sleep
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <errno.h>        // errno
#include <cerrno>
#include <cstring>        // for strerror
#include <string.h>       // for memset
#include <filesystem>     // for exists
#include <fstream>        // for file stuff
#include <sstream>        // for stringstream stuff
#include <vector>         // for vectors, duh
#include <signal.h>       // for the shutdown signal
#include <fcntl.h>        // for fcntl -- to set non-blocking
#include <random>         // for random #

using namespace std;

// flag for shutting the server down -- ends all while loops - would probably be best to find a way to end for loops too
volatile sig_atomic_t shutdown_flag = 0;

// handles the shut down of the server
void signalHandler(int signum) {
    // cerr << "Server received shutdown signal: " << signum << ". Initiating clean shutdown..." << endl;
    shutdown_flag = 1;
}


// manages the player: joining, hosting, and leaving
int main (int argc, char* argv[]) {
    // check that the command line has the right # of params
    if (argc < 3){
        cerr << "Error: Not enough parameters passed in. Usage: " << argv[0] << " <testNumber> <windowSize>\n";
        return 1;
    }

    // 

    return 0;
} 