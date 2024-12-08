// has the game logic fopr two players to play a remote tic tac toe game

// my files
#include "TicTacToeGame.hpp"

// custom constructor - does nothing for now
TicTacToe::TicTacToe() {

}

// custom destructor - does nothing for now
TicTacToe::~TicTacToe() {
    bool over = false;
    // add in intializing later instead of having in the start game ***
    char grid[3][3];
    bool host = false;
    int playerSd = 0;
    struct addrinfo* playerinfo = nullptr;
    int id = 0;
    bool myTurn = false;
}

// print out the rules of the game
void TicTacToe::printRules(){
    cout << "These are the rules of the game:" << endl;
    cout << "Moves are based on the 3x3 grid. 0 0 is the top left corner. Get 3 in a row to win. Host always goes first." << endl;
    cout << "Type in exitGame to leave the current game." << endl;
}

// input move 
// checks if the move is from the host - host is always X, client is always O
bool TicTacToe::processMove(int x, int y, bool host){
    cout << "Processing your move" << endl;
    // check if the host
    if (host){
        // if the spot given is a _ fill it with a X
        if (grid[y][x] == '_'){
            grid[y][x] = 'X';
            // check if this move lets them win
            checkForWin('X');
        } else {
            return false;
        }  
    } else {
        // if the spot given is a _ fill it with a O
        if (grid[y][x] == '_'){
            grid[y][x] = 'O';
            // check if this move lets them win
            checkForWin('O');
        } else {
            return false;
        }
    }
    // print out new grid
    printGrid();
    return true;
}

bool TicTacToe::updateGameState(vector<char> payload){
    // update your grid
    memcpy(grid, payload.data(), 9);
    // update the over state of the game
    this->over = (payload[9] != 0);
    // print out new grid
    printGrid();
    cout << "Your turn" << endl;
    return true;
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
    // move up the rows
    for (int i = 0; i < 3; i++){
        // up the counts for the row
        if (grid[i][2 - i] == mark){
            markCount++;
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
        // send win msg to local
        sendLocalWinMsg();
        // send lost msg to other player
        sendLoseMsg();
    } else if (mark == '_'){
        // local msg
        sendLocalCatsMsg();
        // send cats msg to other player
        sendCatsMsg();
    } else {
        // local msg
        sendLocalLoseMsg();
        // send win msg to the other player
        sendWinMsg();
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

// read in the msgs and pass them off for processing -- updates whoose turn it is 
void TicTacToe::readMsg(){
    // make sure the playerSd is valid before trying to read
    if (this->playerSd <= 0){
        return;
    }
    // read in the msg -- tic protocols related msgs
    struct baseMsg msg = receiveNonblockingTcp(this->playerSd);
//cout << "msg received of type = " << msg.type << endl;
    // make sure there is a msg 
    if (msg.type != 0){
        // check if the msg is an error
        if (msg.type == -1){
//cout << " msg of type -1" << endl;
            // do nothing for now
        } else if (msg.type == -2){
            this->over = true;
        }
        // check if the msg type is a move
        else if (msg.type == 1){
//cout << "got msg of type 1" << endl;
            // put the payload into a sstream
            if(msg.payload.size() == sizeof(int) * 2){
                int x, y;
                memcpy(&x, msg.payload.data(), sizeof(int));
                memcpy(&y, msg.payload.data() + sizeof(int), sizeof(int));
//cout << "got move: " << x << ", " << y << endl;
                // feed it into process move
                if (processMove(x, y, false)){
                    // it's now your turn
                    this->myTurn = !this->myTurn;
                    cout << "Your turn" << endl;
                } else {
                    cout << "The other player did an Invalid move giving them a retry" << endl;
                    sendTryAgainMsg();
                    sendState(this->grid, this->over);
                }
            } else {
                cerr << "ERROR: Move payload size mismatch" << endl;
            }
        } 
        // game state update
        else if (msg.type == 2){
            if (updateGameState(msg.payload)){
                // it's now your turn
                this->myTurn = !this->myTurn;
            } else {
                cout << "Invalid game state" << endl;
            }
        }
         // generic msg/notice to print out 
        else if (msg.type == 3){
            // put payload into string 
            string payload(msg.payload.begin(), msg.payload.end());
            // print out msg
            cout << payload << endl;   
        } else {
            cerr << "ERROR: Unknown game msg type" << endl;
        }
    }
//    cout << "leaving readMsg" << endl;
}

// check for game cmds other than exitGame - currently none exist 
void TicTacToe::checkGameCmds(vector<string> tokens){
    
}

string TicTacToe::getUserInput(){
    // to store the user input
    string input;
    // check for user input
    getline(cin, input);
    return input;
}

vector<string> TicTacToe::tokenizeInput(string input){
    // vector to store the tokens
    vector<string> tokens;
    // put the string into a sstream
    istringstream stream(input);
    // hold the current token
    string token;

    // parse the user input
    while(stream >> token){
        // put curretn token into the vector
        tokens.push_back(token);
    }
    return tokens;
}

// prompt the user for their move, if host process it, if client send it
bool TicTacToe::movePrompt(vector<string> tokens){
    // send msg to user
//    cout << "Your move: ";

    // check the length of tokens
    if (tokens.size() >= 3 || tokens.size() == 1){
        cout << "Move is not 2 coordinates" << endl;
    }
    else if (tokens.size() == 2){
        try {
            // try to convert them into ints
            int x = stoi(tokens[0]);
            int y = stoi(tokens[1]); 
            // check if the coordinates are in range
            if ((x <= 2 && x >= 0) && (y <= 2 && y >=0)){
                // if host process the move
                if (this->host){
                    // process your move
                    if (processMove(x, y, this->host)){
                        // send the new game state/grid to the client
                        sendState(this->grid, this->over);
                        return true;
                    }
                    return false;
                } else {
                    cout << "Sending your move" << endl;
                    // send the move
                    sendMove(x, y);
                    return true;
                }
            }            
        } catch (const invalid_argument& e) {
            cout << "Invalid argument: " << e.what() << endl;
        } catch (const out_of_range& e) {
            cout << "Out of range: " << e.what() << endl;
        }
    } else if (tokens.empty()){
        // do nothing
    } else {
        cout << "Move inputted incorrectly" << endl;
    }
    return false;
}

// closes the connection socket or the listening socket
void TicTacToe::disconnectFromPlayer(int playerSd){
    this->playerSd = 0;
    closeSocket(playerSd);
    cout << "Disconnected from the other player" << endl;
}

// leave the game - handles both cases of the calling player being host and client 
void TicTacToe::exitGameOut(){
    // check if you are in a game
    if (this->id == 0){
        cout << "Your not in a game" << endl;
        return;
    } else {
        cout << "Exiting Game" << endl;
        // reset your game
        this->id = 0;
        this->over = true;
    }
}

// send move - used by the client
void TicTacToe::sendMove(int x, int y){
//cout << "entered send move" << endl;
    // put the data into a char*
    char payload[8];
    memcpy(payload, &x, sizeof(int));
    memcpy(payload + sizeof(int), &y, sizeof(int));
    // make a new base msg
    // 1 = move
    struct baseMsg msg(1, payload, sizeof(payload));
//cout << "about to call sendTCpMsg" << endl;
    sendTcpMsg(this->playerSd, msg);
}

// send game state - used by host 
void TicTacToe::sendState(char grid[3][3], bool over){
    // put the data into a char*
    char payload[10];
    // Copy the grid into the payload buffer
    memcpy(payload, grid, 9);
    // add the over to the end
    payload[9] = over ? 1 : 0;
    // make a new base msg
    // 2 = state
    struct baseMsg msg(2, payload, strlen(payload));
    sendTcpMsg(this->playerSd, msg);
}

// send game win msg - to be sent on the local
void TicTacToe::sendLocalWinMsg(){
    cout << "Congrats you won the match!" << endl;
}

// send game lose msg - to be sent on the local
void TicTacToe::sendLocalLoseMsg(){
    cout << "Whelp you lost" << endl;
}

// send game lose msg - to be sent on the local
void TicTacToe::sendLocalCatsMsg(){
    cout << "No one wins" << endl;
}

// send game win msg - to be sent on the local
void TicTacToe::sendWinMsg(){
    // put the data into a char*
    char payload[32];
    // the msg to send 
    string notice = "Congrats you won the match!";
    snprintf(payload, sizeof(payload), "%s", notice.c_str());
    // make a new base msg
    // 3 = msg/notice to print
    struct baseMsg msg(3, payload, strlen(payload));
    // send the baseMsg
    sendTcpMsg(this->playerSd, msg);
}

// send game lose msg - to be sent on the local
void TicTacToe::sendLoseMsg(){
    // put the data into a char*
    char payload[32];
    // the msg to send 
    string notice = "Whelp you lost";
    snprintf(payload, sizeof(payload), "%s", notice.c_str());
    // make a new base msg
    // 3 = msg/notice to print
    struct baseMsg msg(3, payload, strlen(payload));
    // send the baseMsg
    sendTcpMsg(this->playerSd, msg);
}

// send game lose msg - to be sent on the local
void TicTacToe::sendCatsMsg(){
    // put the data into a char*
    char payload[32];
    // the msg to send 
    string notice = "No one wins";
    snprintf(payload, sizeof(payload), "%s", notice.c_str());
    // make a new base msg
    // 3 = msg/notice to print
    struct baseMsg msg(3, payload, strlen(payload));
    // send the baseMsg
    sendTcpMsg(this->playerSd, msg);
}

void TicTacToe::sendTryAgainMsg(){
    // put the data into a char*
    char payload[32];
    // the msg to send 
    string notice = "Invalid move try again";
    snprintf(payload, sizeof(payload), "%s", notice.c_str());
    // make a new base msg
    // 3 = msg/notice to print
    struct baseMsg msg(3, payload, strlen(payload));
    // send the baseMsg
    sendTcpMsg(this->playerSd, msg);
}

// start a game and the make the player who started it the host -- the game logic "main"
void TicTacToe::startGame(){
    // intialize the grid to be filled with _
    fill(&this->grid[0][0], &this->grid[0][0] + sizeof(this->grid), '_');
    cout << "\nWelcome to high stakes Tic Tac Toe, its high stakes because if you lose everyone knows you're an idiot" << endl;
    cout << "When it is your turn please enter your move as a coordinate point using a space (ex: '1 2'): " << endl;

    if (this->host){
        // give a game start msg
        cout << "The game has started, go first while you wait for another player to join" << endl;
        this->myTurn = true;
    } else {
        cout << "You have joined the game! The host goes first, so you might have to wait for your turn" << endl;
        // read in the connection confirmation msg
        readMsg();
    }
    // print out the empty grid
    printGrid();

    // fd to monitor the input streams
    fd_set readfds;
    // for the timout value
    struct timeval tv;

    // player turn loop
    while (!this->over && !GlobalFlags::shutdown_flag){
        // clear the fd set
        FD_ZERO(&readfds);
        // set to moniter the cin
        FD_SET(STDIN_FILENO, &readfds);
        // set to monitor the socket between players
        FD_SET(this->playerSd, &readfds);

        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        // wait for activity on the sets or until the timeout
        int activity = select(max(STDIN_FILENO, this->playerSd) + 1, &readfds, NULL, NULL, &tv);

        // if activity is found
        if (activity > 0){
            // if the activity is on the cin
            if (FD_ISSET(STDIN_FILENO, &readfds)){
                // check for input from user 
                string input = getUserInput();
                if (!input.empty()){
                    // check for exit game cmd
                    if (input == "exitGame"){
                        // leave 
                        exitGameOut();
                        break;
                    } 
                    // your turn
                    else if (this->myTurn && !this->over){
            //            cout << "it's your turn" << endl;
                        vector<string> tokens = tokenizeInput(input);
                        // ask for a move - also handles the sending of msgs
                        if (movePrompt(tokens)){
                            // its no longer your turn
                            this->myTurn = !this->myTurn;
                            cout << "Other players turn: " << endl;
                        } else {
                            cout << "Invalid move try again" << endl;
                        }
                    } else {
            //cout << "checking other game cmds" << endl;
                        vector<string> tokens = tokenizeInput(input);
                        // check for other cmds
                        checkGameCmds(tokens);
                    }
                }
            } 
            // if activity on the socket between players
            if (FD_ISSET(this->playerSd, &readfds)){
                readMsg();
            }  
        }
    } 
    // game clean up
    disconnectFromPlayer(this->playerSd);
    // reset all of the game vars -- not ideal should find a way to have the destructor be called
    this->over = false;
    this->host = false;
    this->playerSd = 0;
    this->playerinfo = nullptr;
    this->id = 0;
    this->myTurn = false;
}