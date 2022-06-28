#include "player.hpp"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
  int player_nr = atoi(argv[1]);

  cout << "\n\nHi Boss\t\t\t\t\tMac recived " << argc << " parameters" << endl;
  cout << "Macintosh ready for some tests...\tPlaying as P" << player_nr
       << endl;

  try {
    Player p(2);

    int round = 22;
    string board_name;
    board_name = "boards/board_" + to_string(round) + ".txt";

    // cout << "\nT1) Creating the first board...\t\t";
    // p1.init_board(board_name);
    // cout << "Board initialized correctly!" << endl;


    cout << "T2) Trying to load the board...\t\t";
    p.load_board(board_name);
    cout << "Board loaded correctly!" << endl;

    p.move();
    p.valid_move() ? cout << "Valida" << endl : cout << "NON VALIDA!" << endl;

    cout << "T3) Trying to save the board...\t\t";
    board_name = "boards/board_" + to_string(++round) + ".txt";
    p.store_board(board_name, 0);
    cout << "Board saved correctly!" << endl;

    p.loses() ? cout <<  "P1 PERSO" << endl : cout << "NON PERSO" << endl;

  } catch (player_exception &pe) {
    cout << "\n\nError type:\t" << pe.t << "\nException...\t" << pe.msg << endl;
  }

  cout << "\n\nBye bye Boss..." << endl;
  return 0;
}
