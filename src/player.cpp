#include "player.hpp"

#define ROWS    8
#define COLS    15

#define MATSIZE 120
#define MPED    24
#define NPED    12

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
    Node  head;
    Node  tail;

    void  destroy( Node ) const;
    void  append( Node );
    void  printBoard( const Node& );
    void  printMemBoard( const Node& );
    void  listBoards();
    void  listRevBoards();
    Node  copy( const Node& );
    piece findEnum( const char& );
    Node  bringNode( const int& );

    void validBoard( int, size_t, const Node& );
};

/// Player class implementation ///////////////////////////////////////////////

Player::Player( int player_nr )
{
    if( player_nr != 1 && player_nr != 2 ) throw player_exception{ player_exception::index_out_of_bounds, "player_nr is neither player1 or player2 in Player constructor!!" };
    pimpl = new Impl;

    pimpl->nPLayer = (short) player_nr;
    pimpl->head    = nullptr;
    pimpl->tail    = nullptr;
}

Player::~Player()
{
    pimpl->destroy( pimpl->head );
    delete pimpl;
}

Player::Player( const Player& p )
{
    *this = p;
}

Player& Player::operator=( const Player& pl )
{
    if( this != &pl )
    {
        pimpl = new Impl;
        pimpl->destroy( pimpl->head );
        Node moveMe = pl.pimpl->head;

        while( moveMe != nullptr )
        {
            this->pimpl->append( pimpl->copy( moveMe ) );

            moveMe = moveMe->next;
        }
    }

    return *this;
}

void Player::init_board( const string& filename ) const
{
    ofstream initFile;

    initFile.open( filename );
    for( int rows = 0; ( initFile.good() ) && ( rows < ROWS ); ++rows )
    {
        for( int cols = 0; cols < COLS; ++cols )
        {
            if( rows == 3 || rows == 4 )
                initFile << ' ';
            else if( ( rows % 2 == 0 ) && ( cols % 4 == 0 ) )
                rows >= 5 ? initFile << 'x' : initFile << 'o';
            else if( ( rows % 2 != 0 ) && ( cols % 2 == 0 ) && ( cols % 4 != 0 ) )
                rows >= 5 ? initFile << 'x' : initFile << 'o';
            else
                initFile << ' ';
        }
        if( rows < 7 )
            initFile << endl;
    }
    initFile.close();
}


void Player::load_board( const string &filename )
{
    ifstream loadFile;

    loadFile.open( filename );
    if( !loadFile.is_open() ) throw player_exception{ player_exception::missing_file, "Missing file in load_board() func!!..." };

    int i = ( ROWS - 1 );
    string str;
    Node  newNode    = new Cell;
    short xCounter   = 0;
    short oCounter   = 0;
    while( getline( loadFile, str ) )
    {
        if( str.length() != 15 ) throw player_exception { player_exception::missing_file, "Invalid length in load_board() func..." };
        for( size_t j = 0; j < COLS; ++j )
        {
            if( (char)str.at( j ) == ' ')
                str.at( j ) = 'e';

            newNode->board[ i ][ j ] = (char)str.at( j );
            pimpl->validBoard( i, j, newNode );
            switch ( newNode->board[ i ][ j ])
            {
                case 'x': case 'X':
                    ++xCounter;
                    break;

                case 'o': case 'O':
                    ++oCounter;
                    break;

                case 'e':
                    break;

                default:
                    throw player_exception { player_exception::missing_file, "Invalid character in board loaded!!..." };
            }

            if( xCounter + oCounter > MPED )
                throw player_exception { player_exception::invalid_board, "To many pieces in the loaded board!!..." };
        }

        --i;
    }
    loadFile.close();
    pimpl->append( newNode );
}

void Player::store_board( const string &filename, int history_offset ) const
{
    Node moveMe = pimpl->bringNode( history_offset );

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

Player::piece Player::operator()( int r, int c, int history_offset ) const
{
    if( ( r < 0 && r > 7 ) && ( c < 0 && c > 14) ) throw player_exception { player_exception::index_out_of_bounds, "Wrong coordinates... out of range in operator() func..." };
    Node moveMe = pimpl->bringNode( history_offset );

    return pimpl->findEnum( moveMe->board[ r ][ c ] );
}

void Player::move()
{
    pimpl->listBoards();
    // pimpl->listRevBoards();
}

void Player::pop()
{
    if( !pimpl->head ) throw player_exception { player_exception::index_out_of_bounds, "Empty History in pop() func..."};

    if( pimpl->tail->prev )
    {
        Node prevTail = pimpl->tail->prev;
        pimpl->destroy( pimpl->tail );
        pimpl->tail = prevTail;
        prevTail->next = nullptr;
        delete prevTail;
    }
    else
    {
        pimpl->destroy( pimpl->head );
        pimpl->head = nullptr;
        pimpl->tail = nullptr;
    }
}

int Player::recurrence() const
{
    if( pimpl->tail == nullptr ) throw player_exception { player_exception::index_out_of_bounds, "Empty History in recurrence() func..." };

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

bool Player::wins( int player_nr ) const
{
    if( pimpl->tail == nullptr || ( player_nr != 1 && player_nr != 2 ) ) throw player_exception { player_exception::index_out_of_bounds, "player_nr is neither 1 or 2, or empty history in wins() func..." };
    cout << player_nr << endl;
    return true;
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

Node Player::Impl::copy( const Node& node )
{
    Node storeMe = new Cell;

    for( int i = (ROWS - 1); i >= 0; --i )
    {
        for( int j = 0; j < COLS; ++j )
            storeMe->board[ i ][ j ] = node->board[ i ][ j ];
    }

    return storeMe;
}

Node Player::Impl::bringNode( const int& history_offset )
{
    int history = 0;
    Node moveMe = this->tail;

    while( ( moveMe->prev != nullptr ) && ( history != history_offset ) )
    {
        moveMe = moveMe->prev;
        ++history;
    }
    if( history_offset > history ) throw player_exception { player_exception::index_out_of_bounds, "History offset greater than history size!..." };

    return moveMe;
}

Player::piece Player::Impl::findEnum( const char &c )
{
    Player::piece p = Player::x;
    switch( c )
    {
        case 'x':
            p = Player::x;
            break;

        case 'o':
            p = Player::o;
            break;

        case 'e':
            p = Player::e;
            break;

        case 'X':
            p = Player::X;
            break;

        case 'O':
            p = Player::O;
            break;
    }

    return p;
}

void Player::Impl::validBoard( int row, size_t col, const Node& node )
{

    if( ( row % 2 == 1 ) && ( col % 4 != 0 ) && node->board[ row ][ col ] != 'e' )
        throw player_exception { player_exception::missing_file, "Invalid piece position in board loaded!!..." };
    else if( ( row % 2 == 0 ) && ( ( col - 2) % 4 != 0 ) && node->board[ row ][ col ] != 'e' )
        throw player_exception { player_exception::missing_file, "Invalid piece position in board loaded!!..." };
}

void Player::Impl::printBoard( const Node& printNode )
{
    cout << "---------------"<< endl;
    for( int i = ROWS - 1; i >= 0; --i )
    {
         for( int j = 0; j < COLS; ++j )
            printNode->board[ i ][ j ] == 'e' ? cout << ' ' : cout <<  printNode->board[ i ][ j ];

         cout << endl;
    }
    cout << "---------------\n"<< endl;

    // printMemBoard( printNode );
}

void Player::Impl::printMemBoard( const Node& printNode )
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
