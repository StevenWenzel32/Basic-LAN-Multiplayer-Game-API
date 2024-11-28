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

// read in the msgs and pass them off for processing
void readMsg(){
    // read in the msg -- tic protocols related
    struct baseMsg msg = receiveBlockingTcp(this->playerSd);

    // check if the msg type is a move
    if (msg->type == 1){
        // put the payload into a sstream
        istringstream payloadStream(string(msg->payload.begin(), msg->payload.end()));
        // parse the payload
        int x, y;
        if (payloadStream >> x >> y){
            // feed it into process move
            processMove(x, y, host);
        } else {
            cerr << "ERROR: bad move given" << endl;
        }
    } else if (msg->type == 2){
        // put the payload into a sstream
        istringstream payloadStream(string(msg->payload.begin(), msg->payload.end()));
        // set your grid to the grid received
        for (int i = 0; i < 3; i++){
            for (int j = 0; j < 3; j++){
                payloadStream >> grid[i][j];
            }
        }
        
    } else {
        cerr << "ERROR: Unknown game msg type" << endl;
    }
}

// prompt the user for their move, if host process it, if client send it
void movePrompt(){
    // send msg to user
    cout << "Please enter your move as a coordinate point using a space (ex: 1 2): " << endl;
    cout << "Your move: ";

    // to store the user input
    string input;
    // check for user input
    getline(cin, input);
    // put the string into a sstream
    istringStream stream(input);
    // vector to store the tokens
    vector<string> tokens;
    // hold the current token
    string token;

    // parse the user input
    while(stream >> token){
        // put curretn token into the vector
        tokens.push_back(token);
    }

    // if host process the move
    if (host){
        // process your move
        processMove(tokens[0], tokens[1], host);
    } else {
        // send the move
        sendMove(tokens[0], tokens[1]);
    }
}

// start a game and the make the player who started it the host -- the game logic "main"
void startGame(){
    // give a game start msg
    cout << "The game has started" << endl;
    // intialize the grid to be filled with _
    fill(&grid[0][0], &grid[0][0] + sizeof(grid), '_');

    // print out the empty grid
    printGrid();

    // loop through sending and retreiving moves and states -- while the game is still playing
    while (win == 0){
        // other players turn
        // check if there is a move to read in -- blocking
        readMsg();
        // print out new grid
        printGrid();

        // your turn 
        // prompt the user for their move
        movePrompt();
        // print out the new grid
        printGrid();

        // check if host
        if (this.host){
            // send the new game state/grid to the client
            sendState(grid);
        } 
    }
}