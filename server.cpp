// This is basically the lobby -- handles the player connections

using namespace std;

// manages the player: joining, hosting, and leaving
int main (int argc, char* argv[]) {
    // check that the command line has the right # of params
    if (argc < 3){
        cerr << "Error: Not enough parameters passed in. Usage: " << argv[0] << " <testNumber> <windowSize>\n";
        return 1;
    }

    // 

    return 0;
} 