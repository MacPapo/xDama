#include "player.hpp"
#include <iostream>

using namespace std;

int main( int argc, char** argv )
{
    int player_nr = atoi( argv[ 1 ] );

    int round = player_nr;
    string board_name =  "board_" + std::to_string( round ) + ".txt";

    cout << "\n\nHi Boss\t\t\t\t\tMac recived " << argc << " parameters" <<endl;
    cout << "Macintosh ready for some tests...\tPlaying as P" << player_nr << endl;

    try
    {
        Player p1( player_nr );

        // cout << "\nT1) Creating the first board...\t\t";
        // p1.init_board( board_name );
        // cout << "Board initialized correctly!" << endl;

        cout << "T2) Trying to load the board...\t\t";
        p1.load_board( board_name );
        cout << "Board loaded correctly!" << endl;

        // board_name = "board_2.txt";
        // p1.load_board( board_name );
        // board_name = "board_3.txt";
        // p1.load_board( board_name );
        // board_name = "board_4.txt";
        // p1.load_board( board_name );

        // board_name = "board_6.txt";
        // p1.load_board( board_name );
        // board_name = "board_7.txt";
        // p1.load_board( board_name );
        // board_name = "board_8.txt";
        // p1.load_board( board_name );
        // board_name = "board_9.txt";
        // p1.load_board( board_name );

        // cout << "T3) Trying to save the board...\t\t";
        // board_name = "board_9.txt";
        // p1.store_board( board_name, 0 );
        // cout << "Board saved correctly!" << endl;

        // Player p2(p1);
        // p2.move();
        // cout << "\n--------------\n";
        // p1.move();

        // cout << "Popping the last cell...\t";
        // p1.pop();
        // // p2.pop();
        // cout << "Last cell popped" << endl;

        // p2.move();
        // cout << "\n--------------\n";
        p1.move();
        // cout << "T4) Finding recurrent boards...\t\t";
        // cout << "Recurrence founded: " << p1.recurrence() << endl;

        // cout << "T5) Finding the piece...\t\t";
        // cout << "Character found: " << p1( 7, 0 ) << endl;
        //p1.move();
    }
    catch( player_exception& pe )
    {
        cout << "\n\nError type:\t" << pe.t << "\nException...\t" << pe.msg << endl;
    }

    
    cout << "\n\nBye bye Boss..." << endl;
    return 0;
}
