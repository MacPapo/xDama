#include "player.hpp"
#include <iostream>

using namespace std;

int main( int argc, char** argv )
{
    cout << "The executable recived " << argc << " parameters\n" <<endl;
    int player_nr = atoi( argv[ 1 ] );

    int round = player_nr;
    string board_name =  "board_" + std::to_string( round ) + ".txt";
    cout << "Hi Boss, ready for some tests...\nPlaying as P" << player_nr << endl;

    try
    {
        Player p1( player_nr );
        cout << "\nInitializing the first board...\t";
        p1.init_board( board_name );
        cout << "Board initialized correctly!" << endl;
    }
    catch(player_exception)
    {
        cout << "Exception..." << endl;
    }

    
    cout << "\n\nBye bye Boss..." << endl;
    return 0;
}
