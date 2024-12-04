// has the game logic fopr two players to play a remote tic tac toe game

#ifndef TICTACTOE_GAME_HPP
#define TICTACTOE_GAME_HPP

// my files
#include "basicNetworking.hpp"

#include <sstream>

class TicTacToe {
    public: 
    TicTacToe();
    ~TicTacToe();

    // if the game is over or not
    bool over = false;
    // the grid of the game - 3x3
    // _ = empty spot
    // X = host placed
    // O = client placed
    char grid[3][3];
    // used to show if you are hosting a game
    bool host = false;
    // same sd as in the player object but is here for simplicity 
    int playerSd = 0;

    // input move 
    // checks if the move is from the host - host is always X, client is always O
    void processMove(int x, int y);
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
    // prompt the user for their move and feeds it into processMove()
    void movePrompt();
    // start a game and the make the player who started it the host -- the game logic "main"
    void startGame();
    // send move - used by both client and host
    void sendMove(int x, int y);
    // send game state - used by host 
    void sendState(char grid[3][3], bool over);
    // send game win msg - to be sent on the local
    void sendWinMsg();
    // send game lose msg - to be sent on the local
    void sendLoseMsg();
    // send cats game msg - to be sent on the local
    void sendCatsMsg();
};

#endif