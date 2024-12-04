// has the game logic fopr two players to play a remote tic tac toe game

// my files
#include "TicTacToeGame.hpp"

// custom constructor - does nothing for now
TicTacToe::TicTacToe() {

}

// custom destructor - does nothing for now
TicTacToe::~TicTacToe() {

}

// input move 
// checks if the move is from the host - host is always X, client is always O
void TicTacToe::processMove(int x, int y){
    // check if the host
    if (this->host){
        // if the spot given is a _ fill it with a X
        if (grid[y][x] == '_'){
            grid[y][x] = 'X';
            // check if this move lets them win
            checkForWin('X');
            return;
        } else {
            cout << "that is not a valid move" << endl;
        }  
    } else {
        // if the spot given is a _ fill it with a O
        if (grid[y][x] == '_'){
            grid[y][x] = 'O';
            // check if this move lets them win
            checkForWin('O');
            return;
        } else {
            cout << "that is not a valid move" << endl;
        }
    }
}

// go through and check for 3 in a row in the grid for the passed in mark
void TicTacToe::checkForWin(char mark){
    // check if the mark is valid
    if (mark == 'X' || mark == 'O'){
        // loop through the rows - does minor checks for cats game
        checkRows(mark);
        // loop throught the columns - does minor checks for cats game
        checkColumns(mark);
        // check the diagonals
        checkDiagonalLeftRight(mark);
        checkDiagonalRightLeft(mark);
    }
}

// checks if the passed in mark won -- also checks for a cats game
void TicTacToe::checkRows(char mark){
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
    if (spacesLeft == 0){
        // call it a cats game
        gameWin('_');
    }
}

// checks if the passed in mark won -- also checks for a cats game
void TicTacToe::checkColumns(char mark){
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
void TicTacToe::checkDiagonalLeftRight(char mark){
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
void TicTacToe::checkDiagonalRightLeft(char mark){
    // counter for the mark
    int markCount = 0;
    // move down the columns
    for (int j = 2; j > 0; j--){
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
void TicTacToe::gameWin(char mark){
    // mark the game as over
    over = true;
    // if host won
    if (mark == 'X' && this->host){
        sendWinMsg();
    } else if (mark == '_'){
        sendCatsMsg();
    } else {
        sendLoseMsg();
    }
}

// print out the game grid
void TicTacToe::printGrid(){
    // print empty line before start
    cout << endl;
    // loop through the rows
    for (int i = 0; i < 3; i++){
        // loop through the columns
        for (int j = 0; j < 3; j++){
            // print out the current location
            // if in middle column
            if (j == 1){
                cout << "| " << grid[i][j] << " |";
            } else if (j == 2){
                cout << " " << grid[i][j] << " ";
                cout << endl;
                // if not the last row
                if (i != 2){
                    cout << "-----------" << endl;
                }
            } else {
                cout << " " << grid[i][j] << " ";
            } 
        }
    }
    // print empty line at end
    cout << endl;
}

// read in the msgs and pass them off for processing
void TicTacToe::readMsg(){
    // check if the playerSd is empty
    if (this->playerSd == 0){
        // no player has joined yet
        cout << "no player has joined your game yet, please wait..." << endl;
        // loop until they join
        while (this->playerSd == 0){

        }
    }
    // read in the msg -- tic protocols related
    struct baseMsg msg = receiveBlockingTcp(this->playerSd);

    // check if the msg type is a move
    if (msg.type == 1){
        // put the payload into a sstream
        istringstream payloadStream(string(msg.payload.begin(), msg.payload.end()));
        // parse the payload
        int x, y;
        if (payloadStream >> x >> y){
            // feed it into process move
            processMove(x, y);
        } else {
            cerr << "ERROR: bad move given" << endl;
        }
    } else if (msg.type == 2){
        // put the payload into a sstream
        istringstream payloadStream(string(msg.payload.begin(), msg.payload.end() - sizeof(bool)));
        // set your grid to the grid received
        for (int i = 0; i < 3; i++){
            for (int j = 0; j < 3; j++){
                payloadStream >> grid[i][j];
            }
        }
        // grab the bool state of the game
        bool gameState;
        payloadStream >> gameState;
        // update the over state of the game
        this->over = gameState;
    } else {
        cerr << "ERROR: Unknown game msg type" << endl;
    }
}

// prompt the user for their move, if host process it, if client send it
void TicTacToe::movePrompt(){
    // send msg to user
    cout << "Please enter your move as a coordinate point using a space (ex: 1 2): " << endl;
    cout << "Your move: ";

    // to store the user input
    string input;
    // check for user input
    getline(cin, input);
    // put the string into a sstream
    istringstream stream(input);
    // vector to store the tokens
    vector<string> tokens;
    // hold the current token
    string token;

    // parse the user input
    while(stream >> token){
        // put curretn token into the vector
        tokens.push_back(token);
    }

    // check for exit command
    if (tokens[0] == "exit"){
        this->over = true;
    }
    // if host process the move
    else if (this->host){
        // process your move
        processMove(stoi(tokens[0]), stoi(tokens[1]));
    } else {
        // send the move
        sendMove(stoi(tokens[0]), stoi(tokens[1]));
    }
}

// send move - used by the client
void TicTacToe::sendMove(int x, int y){
cout << "entered send move" << endl;
    // check if the playerSd is empty
    if (this->playerSd == 0){
        // no player has joined yet
        cout << "Your move will be sent once a player joins your game, please wait..." << endl;
        // loop until they join
        while (this->playerSd == 0){

        }
    }
    // put the data into a char*
    char payload[128];
    snprintf(payload, sizeof(payload), "%d:%d", x, y);
    // make a new base msg
    // 1 = move
    struct baseMsg msg(1, payload, strlen(payload));
cout << "about to call sendTCpMsg" << endl;
    sendTcpMsg(this->playerSd, msg);
}

// send game state - used by host 
void TicTacToe::sendState(char grid[3][3], bool over){
    cout << "entered send state" << endl;
    // check if the playerSd is empty
    if (this->playerSd == 0){
        // no player has joined yet
        cout << "Your move will be sent once a player joins your game, please wait..." << endl;
        // loop until they join
        while (this->playerSd == 0){

        }
    }
    // put the data into a char*
    char payload[256];
    snprintf(payload, sizeof(payload), "%d:%d", grid, over);
    // make a new base msg
    // 2 = state
    struct baseMsg msg(2, payload, strlen(payload));
    sendTcpMsg(this->playerSd, msg);
}

// send game win msg - to be sent on the local
void TicTacToe::sendWinMsg(){
    cout << "Congrats you won the match!" << endl;
}

// send game lose msg - to be sent on the local
void TicTacToe::sendLoseMsg(){
    cout << "Whelp you lost" << endl;
}

// send game lose msg - to be sent on the local
void TicTacToe::sendCatsMsg(){
    cout << "No one wins" << endl;
}

// start a game and the make the player who started it the host -- the game logic "main"
void TicTacToe::startGame(){
    // give a game start msg
    cout << "The game has started" << endl;
    // intialize the grid to be filled with _
    fill(&this->grid[0][0], &this->grid[0][0] + sizeof(this->grid), '_');

    // print out the empty grid
    printGrid();

    // loop through sending and retreiving moves and states -- while the game is still playing
    while (!this->over){
        // check if you aren't the host
        if (!this->host){
            // other players turn
            // check if there is a move to read in -- blocking
            readMsg();
            // print out new grid
            printGrid();
        }

        // your turn 
        // prompt the user for their move
        movePrompt();
        // print out the new grid
        printGrid();

        // check if host
        if (this->host){
            // send the new game state/grid to the client
            sendState(this->grid, this->over);
        } 
    }
}