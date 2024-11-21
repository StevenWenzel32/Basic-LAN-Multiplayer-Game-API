// These are the basic networking and socket related functions and basic msg sends and recieves
#ifndef BASIC_NETWORK_HPP
#define BASIC_NETWORK_HPP

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
#include <queue>          // for queue, duh
#include <cerrno>
#include <cstring>        // for strerror
#include <vector>
#include <array>
#include <unordered_map>
#include <signal.h>       // for the shutdown signal
#include <fcntl.h>        // for fcntl -- to set non-blocking

using namespace std;

#endif