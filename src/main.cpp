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

        cout << "Trying to load the board...\t";
        p1.load_board( board_name );
        cout << "Board loaded correctly!" << endl;

        board_name = "board_2.txt";
        p1.load_board( board_name );
        board_name = "board_3.txt";
        p1.load_board( board_name );
        board_name = "board_4.txt";
        p1.load_board( board_name );
        board_name = "board_5.txt";
        p1.load_board( board_name );
        board_name = "board_6.txt";
        p1.load_board( board_name );

        // cout << "Trying to save the board...\t";
        // board_name = "board_6.txt";
        // p1.store_board( board_name, 2 );
        // cout << "Board saved correctly!" << endl;

        p1.move();

    }
    catch( player_exception pe )
    {
        cout << "\n\nError type:\t" << pe.t << "\nException...\t" << pe.msg << endl;
    }

    
    cout << "\n\nBye bye Boss..." << endl;
    return 0;
}
