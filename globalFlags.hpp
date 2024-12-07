// this class is for global flags and controls for larger networking projects
#ifndef GLOBAL_FLAGS_HPP
#define GLOBAL_FLAGS_HPP

#include <signal.h>         // for the signal atomic type
#include <iostream>

using namespace std;

class GlobalFlags {
    public: 
        // flag used to shutdown the program - stops the while loops - used in player.cpp and TicTacToe.cpp
        static volatile sig_atomic_t shutdown_flag;

        static void signalHandler(int signum);
};

#endif