// this class is for global flags and controls for larger networking projects

#include "globalFlags.hpp"

// flag used to shutdown the program - stops the while loops - used in player.cpp and TicTacToe.cpp
volatile sig_atomic_t GlobalFlags::shutdown_flag = 0;

// handles the shut down of the listening loop
void GlobalFlags::signalHandler(int signum){
    cout << "Received signal: " << signum << ". Initiating shutdown..." << endl;
    GlobalFlags::shutdown_flag = 1;
}