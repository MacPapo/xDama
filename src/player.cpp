#include "player.hpp"

#define ROWS 8
#define COLS 15

#define NPED 12

using std::ifstream;
using std::ofstream;

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
    short nPLayer;
    Node head;
    Node tail;

    void destroy( Node ) const;
    void append( Node );
    void printBoard( Node );
    void printMemBoard( Node );
    void listBoards();
    void listRevBoards();
};

/// Player class implementation ///////////////////////////////////////////////

Player::Player( int player_nr )
{
    if( player_nr != 1 && player_nr != 2 ) throw player_exception{ player_exception::index_out_of_bounds, "player_nr is neither player1 or player2" };

    pimpl = new Impl;

    pimpl->nPLayer = (short) player_nr;
    pimpl->head = nullptr;
    pimpl->tail = nullptr;
}

Player::~Player()
{
    pimpl->destroy( pimpl->head );
    delete pimpl;
}

Player::Player( const Player& p )
{
    pimpl = new Impl;
    pimpl->head = p.pimpl->head;
    pimpl->tail = p.pimpl->tail;
}

void Player::init_board( const string& filename ) const
{
    ofstream initFile;

    initFile.open( filename );

    for( int rows = ROWS - 1; initFile.good() && rows >= 0; --rows )
    {
        for( int cols = 0; cols < COLS; ++cols )
        {
            if( rows <= 2 && rows >= 0 ) // print from rows 1 to 3 with x and spaces
            {
                if( rows % 2 == 0 ) // print row 2 with x and spaces
                    ( ( cols % 2 == 0 ) && ( cols % 4 != 0 ) ) ? initFile << 'x' : initFile << ' ';

                else // print rows 6 and 8 with x and spaces
                    ( cols % 4 == 0 ) ? initFile << 'x' : initFile << ' ';
            }

            else if( rows <= 7 && rows >= 5 ) // print from rows 6 to 8 with o and spaces
            {
                if( rows % 2 != 0 ) // print rows 1 and 3 with o and spaces
                    ( cols % 4 == 0 ) ? initFile << 'o' : initFile << ' ';

                else // print row 2 with o and spaces
                    ( !( cols % 4 == 0 ) && ( cols % 2 == 0 ) ) ? initFile << 'o' : initFile << ' ';
            }

            else // print the rows 4 and 5 with only spaces
                initFile << ' ';
        }

        if( rows > 0 )
            initFile << std::endl;
    }

    initFile.close();
}

void Player::load_board( const string &filename )
{
    ifstream loadFile;

    loadFile.open( filename );
    if( !loadFile.is_open() ) throw player_exception{ player_exception::missing_file, "Missing file!..." };

    int i = ( ROWS - 1 );
    string str;
    Node newNode = new Cell;

    while( getline( loadFile, str ) )
    {
        for( size_t j = 0; j < COLS; ++j )
        {
            if( (char)str.at( j ) == ' ')
                str.at( j ) = 'e';

            newNode->board[ i ][ j ] = (char)str.at( j );
        }

        --i;
    }
    loadFile.close();

    // validmove??
    pimpl->append( newNode );
}

void Player::store_board( const string &filename, int history_offset ) const
{
    int history = 0;
    Node moveMe = pimpl->tail;

    while( ( moveMe->prev != nullptr ) && ( history != history_offset ) )
    {
        moveMe = moveMe->prev;
        ++history;
    }

    if( history_offset > history ) throw player_exception { player_exception::index_out_of_bounds, "History offset greater than history size" };

    ofstream outputFile;
    outputFile.open( filename );
    for( int i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( int j = 0; j < COLS; ++j )
            moveMe->board[ i ][ j ] == 'e' ? outputFile << ' ' : outputFile << moveMe->board[ i ][ j ];

        if( i > 0 )
            outputFile << endl;
    }
    outputFile.close();
}

void Player::move()
{
    pimpl->listRevBoards();
}

void Player::pop()
{
    Node prevTail = pimpl->tail->prev;

    pimpl->destroy( pimpl->tail );
    pimpl->tail = prevTail;
    prevTail->next = nullptr;

    delete prevTail;
}

int Player::recurrence() const
{
    int counter = 1;
    bool different;
    Node reference = pimpl->tail;
    Node moveMe = pimpl->tail->prev;

    while( moveMe != nullptr )
    {
        different = false;
        for( int i = 0; i < COLS && !different; ++i )
        {
            for( int j = 0; j < ROWS && !different; ++j )
            {
                if( reference->board[ j ][ i ] != moveMe->board[ j ][ i ] )
                    different = true;
            }
        }

        if( !different )
            ++counter;

        moveMe = moveMe->prev;
    }

    return counter;
}

/// End of Player Implementation //////////////////////////////////////////////

/// Impl struct implementation ////////////////////////////////////////////////

void Player::Impl::destroy( Node node ) const
{
    if( node )
    {
        destroy( node->next );
        delete node;
    }
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

    for( int i = ROWS - 1; i >= 0; --i )
    {
         for( int j = 0; j < COLS; ++j )
            printNode->board[ i ][ j ] == 'e' ? cout << ' ' : cout <<  printNode->board[ i ][ j ];

         cout << endl;
    }

    cout << "---------------\n"<< endl;

    //printMemBoard( printNode );
}

void Player::Impl::printMemBoard( Node printNode )
{
    cout << "---------------"<< endl;

    for( int i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( int j = 0; j < COLS; ++j )
           cout <<  printNode->board[ i ][ j ];

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

/// End of Impl implementation ////////////////////////////////////////////////
