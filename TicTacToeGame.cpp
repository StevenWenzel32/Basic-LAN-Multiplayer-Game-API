// has the game logic fopr two players to play a remote tic tac toe game

// my files
#include "TicTacToeGame.hpp"

// input move 
// checks if the move is from the host - host is always X, client is always O
void processMove(int x, int y, bool host){
    // check if the host
    if (host){
        // if the spot given is a _ fill it with a X
        if (grid[x][y] == '_'){
            grid[x][y] = 'X';
            // check if this move lets them win
            checkForWin('X');
            return;
        }  
    } else {
        // if the spot given is a _ fill it with a O
        if (grid[x][y] == '_'){
            grid[x][y] = 'X';
            // check if this move lets them win
            checkForWin('O');
            return;
        }
    }
    // if you get here then the move is invalid
    cout << "Move is not valid" << endl;
}

// go through and check for 3 in a row in the grid for the passed in mark
void checkForWin(char mark){
    // check if the mark is valid
    if (mark == 'X' || mark == 'O'){
        // loop through the rows 
        checkRows(mark);
        // loop throught the columns
        checkColumns(mark);
        // check the diagonals
        checkDiagonalLeftRight(mark);
        checkDiagonalRightLeft(mark);
    }
}

// checks if the passed in mark won -- also checks for a cats game
void checkRows(char mark){
    // counter for the spaces left
    int spacesLeft = 0;

    // counter for current row spaces
    // loop through the rows
    for (int i = 0; i < 3; i++){
        // counter for the mark
        int markCount = 0;
        // loop through the spots in the row
        for (int j = 0; j < 3; j++){
            // up the counts for the row
            if (grid[i][j] == mark){
                markCount++;
            } else if (grid[i][j] == '_'){
                spacesLeft++;
            }
        }
        // check if they won
        if (markCount == 3){
            gameWin(mark);
            return;
        }
    }
    // check if there are any spaces left
    if (spaceLeft == 0){
        // call it a cats game
        gameWin('_');
    }
}

// checks if the passed in mark won -- also checks for a cats game
void checkColumns(char mark){
    // counter for the spaces left
    int spacesLeft = 0;

    // loop through the columns
    for (int i = 0; i < 3; i++){
        // counter for the mark
        int markCount = 0;
        // loop through the rows
        for (int j = 0; j < 3; j++){
            // up the counts for the column
            if (grid[j][i] == mark){
                markCount++;
            } else if (grid[j][i] == '_'){
                spacesLeft++;
            }
        }
        // check if they won
        if (markCount == 3){
            gameWin(mark);
            return;
        }
    }
    // check if there are any spaces left
    if (spacesLeft == 0){
        // call it a cats game
        gameWin('_');
    }
}

// checks if the passed in mark won -- also checks for a cats game
void checkDiagonalLeftRight(char mark){
    // counter for the mark
    int markCount = 0;

    // loop through the diagonal left to right
    for (int i = 0; i < 3; i++){
        // up the counts for the row
        if (grid[i][i] == mark){
            markCount++;
        }
    }
    // check if they won
    if (markCount == 3){
        gameWin(mark);
    }
}

// checks if the passed in mark won -- also checks for a cats game
void checkDiagonalRightLeft(char mark){
    // counter for the mark
    int markCount = 0;
    // move down the columns
    for (int j = 3; j > 0; j--){
        // move up the rows
        for (int i = 0; i < 3; i++){
            // up the counts for the row
            if (grid[i][j] == mark){
                markCount++;
            }
        }
    }
    // check if they won
    if (markCount == 3){
        gameWin(mark);
    }
}

// game win procedure, pass in who won a
void gameWin(char mark){
    // mark the game as over
    over = true;
    // if host won and you are host
    if (mark == 'X' && host){
        sendWinMsg();
    } else if (mark == '_'){
        sendCatsMsg();
    } else {
        sendLoseMsg();
    }
}

// print out the game grid
void printGrid(){
    // print empty line before start
    cout << endl;
    // loop through the rows
    for (int i = 0; i < 3; i++){
        // loop through the columns
        for (int j = 0; j < 3; j++){
            // print out the current location
            // if in middle column
            if (j == 1){
                cout << "| " << grid[i][j] << " |" << endl;
            } else {
                cout << " " << grid[i][j] << " " << endl;
            }
            
            // if not the last row
            if (i != 2){
                cout << "-----------" << endl;
            }
        }
    }
    // print empty line at end
    cout << endl;
}

// start a game and the make the player who started it the host -- the game logic "main"
void startGame(){
    // give a game start msg
    cout << "The game has started" << endl;
    // intialize the grid to be filled with _
    
    // make an empty grid and show it to the player
    printGrid();

    // loop through sending and retreiving moves and states -- while the game is still playing
    while (win == 0){
        // check if host
        if (this.host){
            // other players turn
            // check if there is a move to read in -- blocking
            read
            // process their move
            // print out new grid
            printGrid();

            // your turn 
            // process your move
            processMove(x, y, host);
            // print out the new grid
            printGrid();
            // send the game state
            sendState();
        } 
        // if client
        else {
            // other players turn
            // check if there is a state to read in -- blocking
            read
            // process the game state
            // print out the grid
            printGrid();

            // your turn
            // send your move
            sendMove();
        }
    }
}