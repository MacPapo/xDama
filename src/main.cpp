#include "player.hpp"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
  int player_nr = atoi(argv[1]);

  int round = player_nr;
  string board_name = "boards/board_" + std::to_string(round) + ".txt";

  cout << "\n\nHi Boss\t\t\t\t\tMac recived " << argc << " parameters" << endl;
  cout << "Macintosh ready for some tests...\tPlaying as P" << player_nr
       << endl;

  try {
    Player p1(player_nr);
    Player p2(2);

    cout << "\nT1) Creating the first board...\t\t";
    p1.init_board(board_name);
    cout << "Board initialized correctly!" << endl;

    int i = 1;
    while (i <= 2000) {
      board_name = "boards/board_" + std::to_string(i++) + ".txt";
      cout << "T2) Trying to load the board...\t\t";
      if (i % 2)
        p1.load_board(board_name);
      else
        p2.load_board(board_name);
      cout << "Board loaded correctly!" << endl;

      cout << "Moving the pawns...\n\n";
      if (i % 2)
        p1.move();
      else
        p2.move();
      cout << "Pawns moved!!" << endl;

      cout << "T3) Trying to save the board...\t\t";
      board_name = "boards/board_" + std::to_string(i) + ".txt";
      if (i % 2)
        p1.store_board(board_name, 0);
      else
        p2.store_board(board_name, 0);
      cout << "Board saved correctly!" << endl;
    }
  } catch (player_exception &pe) {
    cout << "\n\nError type:\t" << pe.t << "\nException...\t" << pe.msg << endl;
  }

  cout << "\n\nBye bye Boss..." << endl;
  return 0;
}
