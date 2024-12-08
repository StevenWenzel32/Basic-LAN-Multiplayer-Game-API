// has the game logic fopr two players to play a remote tic tac toe game

#ifndef TICTACTOE_GAME_HPP
#define TICTACTOE_GAME_HPP

// libraries
#include <sstream>
#include <thread>        // For std::thread

// my files
#include "basicNetworking.hpp"
#include "globalFlags.hpp"

class TicTacToe {
    public: 
    TicTacToe();
    ~TicTacToe();

    // if the game is over or not
    bool over = false;
    // the grid of the game - 3x3 - should set to empty ****
    // _ = empty spot
    // X = host placed
    // O = client placed
    char grid[3][3];
    // used to show if you are hosting a game
    bool host = false;
    // same sd as in the player object but is here for simplicity 
    int playerSd = 0;
    // if the above if a udp - this is the other players info - to use UDP would need a seperate UDP listening other than the boradcast one **
    struct addrinfo* playerinfo = nullptr;
    // the game id
    int id = 0;
    // if its your turn
    bool myTurn = false;

    // print out the rules of the game - tic tac toe
    void printRules();
    // input move 
    // checks if the move is from the host - host is always X, client is always O
    bool processMove(int x, int y, bool host);
    bool updateGameState(vector<char> payload);
    // go through and check for 3 in a row in the grid for the passed in mark
    void checkForWin(char mark);
    // checks if the passed in mark won -- also checks for a cats game
    void checkRows(char mark);
    // checks if the passed in mark won -- also checks for a cats game
    void checkColumns(char mark);
    // checks if the passed in mark won
    void checkDiagonalLeftRight(char mark);
    // checks if the passed in mark won
    void checkDiagonalRightLeft(char mark);
    // game win procedure 
    void gameWin(char mark);
    // print out the game grid
    void printGrid();
    // read in the msgs and pass them off for processing
    void readMsg();
    // check for game cmds other than exitGame - currently none exist 
    void checkGameCmds(vector<string> tokens);
    // get user input from cin and tokenize it
    string getUserInput();
    vector<string> tokenizeInput(string input);
    // prompt the user for their move and feed it into processMove()
    bool movePrompt(vector<string> tokens);
    // disconnects the TCP connection with the other player
    void disconnectFromPlayer(int playerSd);
    // leave the game - handles both cases of the calling player being host and client 
    void exitGameOut();
    // send a msg to the other player that you are leaving the game
    void exitGameMsg();
    // start a game and the make the player who started it the host -- the game logic "main"
    void startGame();
    // send move - used by both client and host
    void sendMove(int x, int y);
    // send game state - used by host 
    void sendState(char grid[3][3], bool over);
    // send game win msg - to be sent on the local
    void sendLocalWinMsg();
    // send game lose msg - to be sent on the local
    void sendLocalLoseMsg();
    // send cats game msg - to be sent on the local
    void sendLocalCatsMsg();
    // send game win msg to the other player - used by host
    void sendWinMsg();
    // send game lose msg to the other player - used by host
    void sendLoseMsg();
    // send cats game msg to the other player - used by host
    void sendCatsMsg();
    // send to the client a try again msg - used by host
    void sendTryAgainMsg();
};

#endif