#include "player.hpp"

#define ROWS 8
#define COLS 15

using std::ifstream;
using std::ofstream;

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

    void destroy( Node );
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

void Player::Impl::destroy( Node node )
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
            if( rows <= 2 && rows >= 0 )
            {
                if( rows % 2 == 0 )
                    ( cols % 4 == 0 ) ? initFile << 'o' : initFile << ' ';
                else
                    ( !( cols % 4 == 0 ) && ( cols % 2 == 0 ) ) ? initFile << 'o' : initFile << ' ';
            }
            else if( rows <= 7 && rows >= 5 )
            {
                if( rows % 2 == 0 )
                    ( cols % 4 == 0 ) ? initFile << 'x' : initFile << ' ';
                else
                    ( !( cols % 4 == 0 ) && ( cols % 2 == 0 ) ) ? initFile << 'x' : initFile << ' ';
            }
            else
                initFile << ' ';
        }

        if( rows < 7 )
            initFile << std::endl;
    }

    initFile.close();
}
