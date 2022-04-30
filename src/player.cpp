#include "player.hpp"

#define ROWS 8
#define COLS 15

using std::ifstream;
using std::ofstream;

/* DELETE ME */
using std::cout;
using std::endl;

struct Cell
{
    char board[ ROWS ][ COLS ];
    struct Cell* next;
    struct Cell* prev;
};

typedef struct Cell* Node;

struct Player::Impl
{
    Node head;
    Node tail;

    void destroy( Node ) const;
    void append( Node );
    void printBoard( Node );
    void listBoards();
    void listRevBoards();
};

Player::Player( int player_nr )
{
    if( player_nr != 1 && player_nr != 2 ) throw player_exception{ player_exception::index_out_of_bounds, "player_nr is neither player1 or player2" };

    pimpl = new Impl;
    pimpl->head = nullptr;
    pimpl->tail = nullptr;
}

Player::~Player()
{
    pimpl->destroy( pimpl->head );
    delete pimpl;
}

void Player::Impl::destroy( Node node ) const
{
    if( node )
    {
        destroy( node->next );
        delete node;
    }
}

void Player::init_board( const string& filename ) const
{
    ofstream initFile;

    initFile.open( filename );

    for( int rows = 0; initFile.good() && rows < ROWS; ++rows )
    {
        for( int cols = 0; cols < COLS; ++cols )
        {
            if( rows <= 2 && rows >= 0 ) // print from rows 1 to 3 with o and spaces
            {
                if( rows % 2 == 0 ) // print rows 1 and 3 with o and spaces
                    ( cols % 4 == 0 ) ? initFile << 'o' : initFile << ' ';

                else // print row 2 with o and spaces
                    ( !( cols % 4 == 0 ) && ( cols % 2 == 0 ) ) ? initFile << 'o' : initFile << ' ';
            }

            else if( rows <= 7 && rows >= 5 ) // print from rows 6 to 8 with x and spaces
            {
                if( rows % 2 == 0 ) // print row 7 with x and spaces
                    ( cols % 4 == 0 ) ? initFile << 'x' : initFile << ' ';

                else // print rows 6 and 8 with x and spaces
                    ( !( cols % 4 == 0 ) && ( cols % 2 == 0 ) ) ? initFile << 'x' : initFile << ' ';
            }

            else // print the rows 4 and 5 with only spaces
                initFile << ' ';
        }

        if( rows < 7 )
            initFile << std::endl;
    }

    initFile.close();
}

void Player::load_board( const string &filename )
{
    ifstream loadFile;

    loadFile.open( filename );
    if( loadFile.bad() ) throw player_exception{ player_exception::missing_file, "Missing file!..." };

    int i = 0;
    string str;
    Node newNode = new Cell;

    while( getline( loadFile, str ) )
    {
        for( size_t j = 0; j < COLS; ++j )
            newNode->board[ i ][ j ] = (char)str.at( j );

        ++i;
    }

    loadFile.close();

    pimpl->append( newNode );
}

void Player::store_board( const string &filename, int history_offset ) const
{
    cout << "\nFilename: "<< filename << "\nHistory: "<< history_offset << '\n' << endl;

    pimpl->listBoards();
}

void Player::Impl::append( Node newNode )
{
    if( head == nullptr )
    {
        head = newNode;
        tail = newNode;
        newNode->prev = nullptr;
    }
    else
    {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }

    newNode->next = nullptr;
}

void Player::Impl::printBoard( Node printNode )
{
    cout << "---------------"<< endl;

    for( int i = 0; i < ROWS; ++i )
    {
         for( int j = 0; j < COLS; ++j )
            cout << printNode->board[ i ][ j ];

         cout << endl;
    }

    cout << "---------------\n"<< endl;
}

void Player::Impl::listBoards()
{
    if( head == nullptr )
        cout << "Sorry no boards loaded yet..." << endl;
    else
    {
        for( Node moveMe = head; moveMe != nullptr; moveMe = moveMe->next )
            printBoard( moveMe );
    }
}

void Player::Impl::listRevBoards()
{
    if( head == nullptr )
        cout << "Sorry no boards loaded yet..." << endl;
    else
    {
        for( Node moveMe = tail; moveMe != nullptr; moveMe = moveMe->prev )
            printBoard( moveMe );
    }
}

void Player::move()
{
    pimpl->listBoards();
}
