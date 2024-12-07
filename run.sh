# this is the demo file to run all test cases for the UDP server and client
g++ -pthread main.cpp player.cpp basicNetworking.cpp TicTacToeGame.cpp globalFlags.cpp -o tictactoe
# being run in an empty environment to try and resolve cin errors 
env -i ./tictactoe