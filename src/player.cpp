#include "player.hpp"
#include <iostream>

#define ROWS    8
#define COLS    15

#define MATSIZE 120
#define MPED    24
#define NPED    12

#define PINF    10000
#define MINF   -10000

using std::ifstream;
using std::ofstream;

using std::cout;
using std::endl;

struct Cell;

struct Bcell;
typedef Bcell* Blist;

struct Bin;
typedef Bin* Pbin;

short selected_player;

// Class Pawn definition
class Pawn
{
    public:
        // Constructors
        Pawn();
        Pawn( const Pawn& );
        Pawn( const char&, const short&, const short&, const short& );

        // Operators
        Pawn operator = ( const Pawn& );
        bool operator == ( const Pawn& );

        // Getters
        short  get_col() const;
        short  get_row() const;
        short  get_player() const;
        char   get_type() const;

        // Setters
        void set_values( const char&, const short&, const short& );
        void set_coordinates( const short&, const short& );
        void set_queen();

        // Bool
        bool is_queen() const;
        bool verify_coordinates( short, short );

        // Pawn destructor
        void del_pawn();

    private:
        char  type;        // Type of Pawn
        bool  queen;       // is it a Pawn or a Queen?
        short player;      // Define the Pawn ownership
        short x;           // X coordinate
        short y;           // Y coordinate
};

// Class Board definition
class Board
{
    public:
        // Constructors
        Board();
        Board( char b[ ROWS ][ COLS ] );
        Board( Board& );

        // Operator
        Board& operator = ( const Board& );

        // DELETE ME
        void   print_board();

        // Getters
        short  get_xleft();
        short  get_oleft();
        float  get_score();
        void   get_all_moves(short);
        void   get_valid_moves( Pawn& );


        // Setters
        void   set_score(float);

        void   move_piece( Pawn& ,short, short );
        void   update_pieces();

        Pawn& at( short, short );
        const Pawn  at( short, short ) const;

        void   traverse_left( short, short, short, const Pawn&, short, std::pair< short, short> );
        void   traverse_right( short, short, short, const Pawn&, short, std::pair< short, short> );

        // Win utility
        short  wins();

        // Board score evaluation
        float  eval_board();

        // Utility for validation
        short  dama_counter(short);
        short  board_diff();

        // Utility for lists
        void   destroy( Blist& ) const;
        void   prepend( Blist& cell, const Board& moved );
        void   append( Blist& cell, const Board& moved);

        // head of dummy boards
        Blist head;

    private:
        short xleft;
        short oleft;
        short xqueen;
        short oqueen;
        float score;
        Pawn  board[ ROWS ][ COLS ];
};
typedef Board* Pboard;

// Struct cell for lists
struct Cell
{
    Board b;
    struct Cell* next;
    struct Cell* prev;
};
typedef Cell* Pcell;

struct Bcell
{
    Board current;
    Blist next;
};

struct Bin
{
    Blist head;
    Bin* nextTrash;
};

// Impl Definition
struct Player::Impl
{
    // Player number
    short num;

    // Main DL list
    Pcell head;
    Pcell tail;

    // DELETE ME
    void   print_board( const Pcell& );

    // Select the right Enum
    piece  select_enum( const char& );

    // Utility funcs for lists
    void  destroy( Pcell& ) const;
    void   append( Pcell );
    Pcell  copy( Pcell& );

    Pcell  shearch_history( const int& );

    bool verify_board( Pcell& );

    // Minimax Algo
    Pboard minimax( const Pboard&, short, bool, float, float );

    // Helper list for minimax algo
    Pbin garbageCollector;
    Pbin garbageCollectorHead;
    Pbin garbageCollectorTail;

    // Utility funcs for Helper list
    void appendToBin( Blist& );
    void destroyBin( Pbin );
    void destroyTrash( Blist );
};

struct Prediction
{
    Pawn before;
    Pawn after;
};

/// Start of Pawn class implementation ////////////////////////////////////////

Pawn::Pawn()
{
    type  = 'e';
    queen = false;
    x     = 0;
    y     = 0;
}

Pawn::Pawn( const char& kind, const short& row, const short& col, const short& player_number )
{
    type   = kind;
    player = player_number;
    x      = col;
    y      = row;

    if( type == 'O' || type == 'X' )
        queen = true;
}

Pawn::Pawn( const Pawn& copy_pawn )
{
    type   = copy_pawn.type;
    queen  = copy_pawn.queen;
    player = copy_pawn.player;
    x      = copy_pawn.x;
    y      = copy_pawn.y;
}

void Pawn::set_values( const char& kind, const short& row, const short& col )
{
    // Set the Ownership
    if( kind == 'O' || kind == 'X' )
    {
        queen = true;
        kind == 'X' ? player = 1 : player = 2;
    }
    else if( kind != 'e')
    {
        queen = false;
        kind == 'x' ? player = 1 : player = 2;
    }
    else // 'e' case
    {
        queen = false;
        player = 0;
    }
    set_coordinates( row, col );
    type = kind;
}

void Pawn::del_pawn()
{
    player =  0;
    type   = 'e';
    queen  = false;
}

void Pawn::set_queen()
{
    if( queen == false )
    {
        type == 'o' ? type = 'O': type = 'X';
        queen = true;
    }
}

void Pawn::set_coordinates( const short& row, const short& col )
{
    if( verify_coordinates( row, col ) )
    {
        x = col;
        y = row;
    }
}

// CONTROLLAMI
bool Pawn::verify_coordinates( short row, short col )
{
    bool is_inside_mat;
    ( ( row >= 0 && row < ROWS ) && ( col >= 0 && col < COLS ) ) ? is_inside_mat = true : is_inside_mat = false;
    return is_inside_mat;
}

bool Pawn::is_queen() const
{
    bool is_queen;
    this->queen ? is_queen = true : is_queen = false;
    return is_queen;
}

short Pawn::get_col() const
{
    return x;
}

short Pawn::get_row() const
{
    return y;
}

char Pawn::get_type() const
{
    return type;
}

short Pawn::get_player() const
{
    return player;
}

Pawn Pawn::operator=( const Pawn& copy_pawn )
{
    player = copy_pawn.player;
    queen  = copy_pawn.queen;
    type   = copy_pawn.type;
    return *this;
}

bool Pawn::operator==( const Pawn& comp_pawn )
{
    bool is_equal;
    type == comp_pawn.get_type() && y == comp_pawn.get_row() && x == comp_pawn.get_col() && player == comp_pawn.get_player() ? is_equal = true : is_equal = false;
    return is_equal;
}

Board::Board()
{
    head = nullptr;
    for( short i( 0 ); i < ROWS; i++ )
    {
        for( short j( 0 ); j < COLS; j++ )
        {
            this->at( i, j ).set_values( 'e', i, j );
        }
    }
    update_pieces();
    score = this->eval_board();
}

Board::Board( char copy_board[ ROWS ][ COLS ] )
{
    head = nullptr;
    for( short i( 0 ); i < ROWS; i++ )
    {
        for( short j( 0 ); j < COLS; j++ )
        {
            this->at( i, j ).set_values( copy_board[ i ][ j ], i, j );
        }
    }
    update_pieces();
    score = this->eval_board();
}

Board::Board( Board& copy_board )
{
    head = nullptr;
    for( short i( 0 ); i < ROWS; i++ )
    {
        for( short j( 0 ); j < COLS; j++ )
        {
            this->at( i, j ).set_values( copy_board.at( i, j ).get_type(), i, j );
        }
    }
    update_pieces();
    score = this->eval_board();
}

Pawn& Board::at( short row, short col )
{
    return board[ row ][ col ];
}

const Pawn Board::at( short row, short col ) const
{
    return board[ row ][ col ];
}

Board& Board::operator=( const Board& eq_board )
{
    xleft  = eq_board.xleft;
    oleft  = eq_board.oleft;
    xqueen = eq_board.xqueen;
    oqueen = eq_board.oqueen;
    score  = eq_board.score;
    for( short i( 0 ); i < ROWS; i++ )
    {
        for( short j( 0 ); j < COLS; j++ )
        {
            this->at( i, j ).set_values( eq_board.at( i, j ).get_type(), i, j );
        }
    }
    return *this;
}

void Board::print_board()
{
    cout << "---------------"<< endl;
    for( short i = ( ROWS - 1 ); i >= 0; --i )
    {
         for( short j = 0; j < COLS; ++j )
            board[ i ][ j ].get_type() == 'e' ? cout << ' ' :  cout << board[ i ][ j ].get_type();
         cout << endl;
    }
    cout << "---------------\n"<< endl;
}

void Board::move_piece( Pawn& piece, short row, short col )
{
    std::swap( this->at( row, col ), piece );
    if( row == 0 || row == ( ROWS - 1 ) )
        this->at( row, col ).set_queen();
}

short Board::dama_counter( short num )
{
    char kind;
    short res( 0 );
    short row( 0 );

    num == 1 ? row = 7 : row = 0;
    row == 0 ? kind = 'O' : kind = 'X';

    for( short i( 0 ); i < COLS; i++ )
    {
        if( this->at( row, i ).get_type() == kind )
            ++res;
        else
        {
            if( ( this->at( row, i ).get_type() == 'o' && row == 0 ) || ( this->at( row, i ).get_type() == 'x' && row == 7 ) )
                return -1;
        }
    }
    return res;
}

void Board::update_pieces()
{
    xleft = xqueen = 0;
    oleft = oqueen = 0;

    for( short i( 0 ); i < ROWS; i++ )
    {
        for( short j( 0 ); j < COLS; j++ )
        {
            switch ( this->at( i, j ).get_type() )
            {
                case 'x':
                     ++xleft;
                     break;
                case 'o':
                    ++oleft;
                    break;
                case 'X':
                    ++xqueen;
                    break;
                case 'O':
                    ++oqueen;
                    break;
                case 'e':
                    break;
                default:
                    throw player_exception { player_exception::missing_file, "Invalid character in board loaded!!..." };
            }
        }
    }
    if( ( ( xleft + xqueen ) + ( oleft + oqueen ) ) > MPED ) throw player_exception { player_exception::invalid_board, "To many pieces in the loaded board!!..." };
}

void Board::prepend( Blist& list_head, const Board& curr_board )
{
    Blist new_cell    = new Bcell;
    new_cell->current = curr_board;
    new_cell->next    = list_head;
    list_head         = new_cell;
}

void Board::append( Blist& list_head, const Board& curr_board )
{
    if( list_head )
        append( list_head->next, curr_board );
    else
        prepend( list_head, curr_board );
}

short Board::get_xleft()
{
    return (short)(xleft + xqueen);
}


short Board::get_oleft()
{
    return (short)(oleft + oqueen);
}

// SEGNALIBRO
short Board::wins()
{
    if( ( oleft + oqueen ) == 0 )
        return 1;
    else if( ( xleft + xqueen ) == 0 )
        return 2;
    return 0;
}

float Board::eval_board()
{
  float mult = 0.5f;
  float res;
  selected_player == 1 ? res = (  ( float )xleft - ( float )oleft + (  xqueen * mult - oqueen * mult ) ) : res = (  ( float )oleft - ( float )xleft + (  oqueen * mult - xqueen * mult ) );
  return res;
}

void Board::set_score( float n )
{
    score = n;
}

void Board::traverse_right( short start, short stop, short step, const Pawn& pawn, short right,  std::pair< short, short > lastPos )
{
    bool eatable = false;
    bool moved   = false;
    bool blocked = false;

    for( short row = start; row < ROWS && row != stop && !moved; row = (short)(row + step) )
    {
        if( right >= COLS )
            break;
        Pawn current = this->at( row, right );
        if( current.get_type() == 'e' && !blocked )
        {
            Board choice = *this;
            choice.move_piece( choice.at( pawn.get_row(), pawn.get_col()), current.get_row(), current.get_col() );
            if( eatable )
            {
                choice.at( lastPos.first, lastPos.second ).del_pawn();
                choice.update_pieces();
            }
            append( head, choice );
            moved = true;
        }
        else if( current.get_player() == pawn.get_player() || ( current.is_queen() && !pawn.is_queen() ) )
            blocked = true;
        else
        {
            eatable = true;
            lastPos.first = current.get_row();
            lastPos.second = current.get_col();
        }
        right = (short)(right + 2);
    }
}

void Board::traverse_left( short start, short stop, short step, const Pawn& pawn, short left, std::pair< short, short > lastPos )
{
    bool eatable = false;
    bool moved   = false;
    bool blocked = false;
    for( short row = start; row < ROWS && row != stop && !moved; row = (short)( row + step ) )
    {
        if( left < 0 )
            break;
        Pawn current = this->at( row, left );
        if( current.get_type() == 'e' && !blocked )
        {
            Board choice = *this;
            choice.move_piece( choice.at( pawn.get_row(), pawn.get_col() ), current.get_row(), current.get_col() );
            if( eatable )
            {
                choice.at( lastPos.first, lastPos.second ).del_pawn();
                choice.update_pieces();
            }
            append( head, choice );
            moved = true ;
        }
        else if( ( current.get_player() == pawn.get_player() ) || ( current.is_queen() && !pawn.is_queen() ) )
            blocked = true;
        else
        {
            eatable = true;
            lastPos.first = current.get_row();
            lastPos.second = current.get_col();
        }
        left = ( short )( left - 2 );
    }
}

void Board::get_valid_moves( Pawn& p )
{
    short   left  = (short)(p.get_col() - 2);
    short   right = (short)(p.get_col() + 2);
    short   row   = p.get_row();
    if( p.get_type() == 'x' || p.is_queen() )
    {
        traverse_left ( (short)(row + 1), (short)( std::max( row + 3, -1 ) ), 1, p, left,  std::pair<short, short>( 0, 0 ) );
        traverse_right( (short)(row + 1), (short)( std::max( row + 3, -1 ) ), 1, p, right, std::pair<short, short>( 0, 0 ) );
    }
    if( p.get_type() == 'o' || p.is_queen() )
    {
        traverse_left ( (short)(row - 1), (short)(std::max( row - 3, -1 ) ), -1, p, left,  std::pair<short, short>( 0, 0 ) );
        traverse_right( (short)(row - 1), (short)(std::max( row - 3, -1 ) ), -1, p, right, std::pair<short, short>( 0, 0 ) );
    }
}

void Board::get_all_moves( short player )
{
    for(short i(ROWS - 1); i >= 0; --i)
        for(short j(0); j < COLS; ++j)
            if( board[i][j].get_player() == player )
                get_valid_moves( board[ i ][ j ] );
}

float Board::get_score()
{
    return score;
}

void Board::destroy( Blist& cell ) const
{
    if(cell)
    {
        destroy(cell->next);
        delete cell;
    }
}


/// End Board class implementation ////////////////////////////////////////////

/// Player class implementation ///////////////////////////////////////////////

Player::Player( int player_nr )
{
    if( player_nr != 1 && player_nr != 2 ) throw player_exception{ player_exception::index_out_of_bounds, "player_nr is neither player1 or player2 in Player constructor!!" };
    pimpl                       = new Impl;
    pimpl->num           = (short) player_nr;
    selected_player             = (short)player_nr;
    pimpl->head                 = nullptr;
    pimpl->tail                 = nullptr;
    pimpl->garbageCollector     = nullptr;
    pimpl->garbageCollectorHead = nullptr;
    pimpl->garbageCollectorTail = nullptr;
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

Player& Player::operator = ( const Player& pl )
{
    if( this != &pl )
    {
        pimpl = new Impl;
        pimpl->destroy( pimpl->head );
        Pcell moveMe = pl.pimpl->head;
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
    for( short rows = 0; ( initFile.good() ) && ( rows < ROWS ); ++rows )
    {
        for( short cols = 0; cols < COLS; ++cols )
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
    char supportBoard[ROWS][COLS];
    string str;
    Pcell newPcell = new Cell;
    short i        = ( ROWS - 1 );
    while( getline( loadFile, str ) )
    {
        if( str.length() != 15 ) throw player_exception { player_exception::invalid_board, "Invalid length in load_board() func..." };
        for( size_t j = 0; j < COLS; ++j )
        {
            if( (char)str.at( j ) == ' ')
                str.at( j ) = 'e';
            supportBoard[ i ][ j ] = (char)str.at( j );
        }
        --i;
    }
    loadFile.close();
    Board sup(supportBoard);
    newPcell->b = sup;
    newPcell->b.update_pieces();
    if(!pimpl->verify_board( newPcell ) || newPcell->b.get_xleft() > 12 || newPcell->b.get_oleft() > 12 ) throw player_exception { player_exception::invalid_board, "Invalid Board in load_board() func..." };
    pimpl->append( newPcell );
}

bool Player::valid_move() const
{
    if(!pimpl->tail->prev) throw player_exception { player_exception::index_out_of_bounds, "Too few boards in History in valid_move() func..." };

    // Catch dama
    short damaXCounterBef = pimpl->tail->prev->b.dama_counter(1);
    short damaXCounterAft = pimpl->tail->b.dama_counter(1);
    short damaOCounterBef = pimpl->tail->prev->b.dama_counter(2);
    short damaOCounterAft = pimpl->tail->b.dama_counter(2);

    cout << "DXB: " << damaXCounterBef << endl
         << "DXA: " << damaXCounterAft << endl
         << "DOB: " << damaOCounterBef << endl
         << "DOA: " << damaOCounterAft << endl;

    if(damaXCounterBef == -1 || damaXCounterAft == -1 || damaOCounterBef == -1 || damaOCounterAft == -1)
        return false;
    if(damaXCounterAft - damaXCounterBef > 1)
        return false;
    if(damaOCounterAft - damaOCounterBef > 1)
        return false;
    // end catch

    short diff = 0;
    Prediction difference[4];
    for(short i = 0; i < ROWS && diff < 4; i++)
    {
        for(short j = 0; j < COLS; j++)
        {
            if(pimpl->tail->b.at(i, j).get_type() != pimpl->tail->prev->b.at(i, j).get_type())
            {
                difference[diff].before = pimpl->tail->prev->b.at(i, j);
                difference[diff].before.set_coordinates(i, j);
                difference[diff].after = pimpl->tail->b.at(i, j);
                difference[diff++].after.set_coordinates(i,j);
            }
        }
    }
    if(diff < 2 || diff > 3)
        return false;

    if(diff == 2)
    {
        Pawn pr { difference[0].before.get_type() != 'e' ? difference[0].before : difference[1].before };
        Pawn af { difference[1].after.get_type() != 'e' ? difference[1].after : difference[0].after };

        Pcell pc { pimpl->tail->prev };
        pc->b.head = nullptr;
        pc->b.get_valid_moves(pr);
        auto moveMe = pc->b.head;
        while(moveMe != nullptr)
        {
            if(moveMe->current.at(af.get_row(), af.get_col()) == af)
            {
                pc->b.destroy(pc->b.head);
                return true;
            }
            moveMe = moveMe->next;
        }
        pc->b.destroy(pc->b.head);
        return false;
    }
    else
    {
        Pawn pr { difference[0].before.get_type() != 'e' ? difference[0].before : difference[2].before };
        Pawn af { difference[2].after.get_type() != 'e' ? difference[2].after : difference[0].after };

        Pcell pc { pimpl->tail->prev };
        pc->b.head = nullptr;
        pc->b.get_valid_moves(pr);
        auto moveMe = pc->b.head;
        while(moveMe != nullptr)
        {
            if(moveMe->current.at(af.get_row(), af.get_col()) == af)
            {
                pc->b.destroy(pc->b.head);
                return true;
            }
            moveMe = moveMe->next;
        }
        pc->b.destroy(pc->b.head);
        return false;
    }
    return false;
}

void Player::move()
{
    if(!pimpl->head) throw player_exception { player_exception::index_out_of_bounds, "Empty History in move() func..." };

    Pboard bestMove(nullptr);

    pimpl->garbageCollector     = nullptr;
    pimpl->garbageCollectorHead = nullptr;
    pimpl->garbageCollectorTail = nullptr;

    bestMove =  pimpl->minimax( &pimpl->tail->b, 4, true, MINF, PINF );
    bestMove->print_board();

    Pcell new_cell = new Cell;
    new_cell->b = *bestMove;
    pimpl->verify_board( new_cell );
    pimpl->append( new_cell );

    if( pimpl->garbageCollector != nullptr )
        pimpl->destroyBin(pimpl->garbageCollector);
}

void Player::store_board( const string &filename, int history_offset ) const
{
    Pcell moveMe = pimpl->shearch_history( history_offset );
    ofstream outputFile;
    outputFile.open( filename );
    for( short i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( short j = 0; j < COLS; ++j )
            moveMe->b.at(i, j).get_type() == 'e' ? outputFile << ' ' : outputFile << moveMe->b.at(i, j).get_type();
        if( i > 0 )
            outputFile << endl;
    }
    outputFile.close();
}

Player::piece Player::operator()( int r, int c, int history_offset ) const
{
    if( ( r < 0 || r > 7 ) || ( c < 0 || c > 14) ) throw player_exception { player_exception::index_out_of_bounds, "Wrong coordinates... out of range in operator() func..." };
    Pcell moveMe = pimpl->shearch_history( history_offset );
    return pimpl->select_enum( moveMe->b.at( (short)r, (short)c ).get_type());
}

void Player::pop()
{
    if( !pimpl->head ) throw player_exception { player_exception::index_out_of_bounds, "Empty History in pop() func..."};
    if( pimpl->tail->prev )
    {
        Pcell prevTail = pimpl->tail->prev;
        pimpl->destroy( pimpl->tail );
        pimpl->tail    = prevTail;
        prevTail->next = nullptr;
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
    bool  different;
    short counter   = 1;
    Pcell reference = pimpl->tail;
    Pcell moveMe    = pimpl->tail->prev;
    while( moveMe != nullptr )
    {
        different = false;
        for( short i = ( ROWS - 1 ); i >= 0 && !different; --i )
            for( short j = 0; j < COLS && !different; ++j )
                if( reference->b.at(i, j).get_type() != moveMe->b.at(i, j).get_type())
                    different = true;
        if( !different )
            ++counter;
        moveMe = moveMe->prev;
    }
    return counter;
}

bool Player::wins( int player_nr ) const
{
    if( pimpl->tail == nullptr ) throw player_exception { player_exception::index_out_of_bounds, "Empty history in wins() or loses() func..." };
    if(pimpl->tail->b.wins() == player_nr)
        return true;
    return false;
}

bool Player::wins() const
{
    return wins(pimpl->num);
}

bool Player::loses( int player_nr ) const
{
    if(player_nr == 1)
        return wins(2);
    return wins(1);
}

bool Player::loses() const
{
    return loses(pimpl->num);
}

/// End of Player Implementation //////////////////////////////////////////////

/// Impl struct implementation ////////////////////////////////////////////////

Player::piece Player::Impl::select_enum( const char& c )
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
        default:
            p = Player::e;
            break;
    }
    return p;
}

bool Player::Impl::verify_board( Pcell& node )
{
    for( short i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( short j = 0; j < COLS; ++j )
        {
            if( ( i % 2 == 1 ) && ( j % 4 != 0 ) && node->b.at(i, j).get_type() != 'e' )
                throw player_exception { player_exception::missing_file, "Invalid piece position in board loaded!!..." };
            else if( ( i % 2 == 0 ) && ( (j - 2) % 4 != 0 ) && node->b.at(i, j).get_type() != 'e' )
                throw player_exception { player_exception::missing_file, "Invalid piece position in board loaded!!..." };
        }
    }
    return true;
}

void Player::Impl::print_board( const Pcell& printCell )
{
    cout << "---------------"<< endl;
    for( short i = ( ROWS - 1 ); i >= 0; --i )
    {
         for( short j = 0; j < COLS; ++j )
            printCell->b.at(i, j).get_type() == 'e' ? cout << ' ' : cout <<  printCell->b.at(i, j).get_type();
         cout << endl;
    }
    cout << "---------------\n"<< endl;
}

Pcell Player::Impl::shearch_history( const int& history_offset )
{
    short history = 0;
    Pcell moveMe = this->tail;
    while( ( moveMe->prev != nullptr ) && ( history != history_offset ) )
    {
        moveMe = moveMe->prev;
        ++history;
    }
    if( history_offset > history ) throw player_exception { player_exception::index_out_of_bounds, "History offset greater than history size!..." };
    return moveMe;
}

void Player::Impl::destroy( Pcell& node ) const
{
    if( node )
    {
        destroy( node->next );
        delete node;
    }
}

void Player::Impl::append( Pcell newPcell )
{
    if( head == nullptr )
    {
        head           = newPcell;
        tail           = newPcell;
        newPcell->prev = nullptr;
    }
    else
    {
        tail->next     = newPcell;
        newPcell->prev = tail;
        tail           = newPcell;
    }
    newPcell->next = nullptr;
}

Pcell Player::Impl::copy( Pcell& node )
{
    Pcell storeMe = new Cell;
    storeMe->b    = node->b;
    return storeMe;
}

Pboard Player::Impl::minimax( const Pboard& base, short depth, bool maxPg, float alpha, float beta )
{
    if(depth == 0 || base->wins() != 0)
        return base;
    if( maxPg )
    {
        Pboard eval(nullptr);
        float maxEval = MINF;
        Blist bestMove(nullptr);
        base->get_all_moves(num);
        Blist move = base->head;
        appendToBin(base->head);
        for( ; move != nullptr; move = move->next )
        {
            eval = minimax( &move->current, (short)( depth - 1 ), false, alpha, beta );
            if( eval )
            {
                maxEval = std::max( maxEval, eval->get_score() );
                alpha = std::max( alpha, maxEval );
                if (beta <= alpha)
                    break;
                if( maxEval == eval->get_score() )
                {
                    bestMove = move;
                    bestMove->current.set_score(eval->get_score());
                }
            }
        }
        if( bestMove == nullptr )
            return base;
        return &bestMove->current;
    }
    else
    {
        Pboard eval( nullptr );
        Blist bestMove( nullptr );
        float minEval = PINF;
        num == 2 ? base->get_all_moves(2) : base->get_all_moves(1);
        Blist move = base->head;
        appendToBin(base->head);
        for(;move != nullptr; move = move->next)
        {
            eval = minimax( &move->current, (short )( depth - 1 ), true, alpha, beta );
            if( eval )
            {
                minEval = std::min( minEval, eval->get_score() );
                beta = std::min( beta, minEval );
                if( beta <= alpha )
                    break;
                if( minEval == eval->get_score())
                {
                    bestMove = move;
                    bestMove->current.set_score(eval->get_score());
                }
            }
        }
        if( bestMove == nullptr )
            return base;
        return &bestMove->current;
    }
}

/// End of Impl implementation ////////////////////////////////////////////////
void Player::Impl::appendToBin( Blist& cell )
{
    if(garbageCollector == nullptr)
    {
        garbageCollector                = new Bin;
        garbageCollector->head          = cell;
        garbageCollectorHead            = garbageCollector;
        garbageCollectorTail            = garbageCollector;
        garbageCollectorTail->nextTrash = nullptr;
    }
    else
    {
        Bin* newTrash        = new Bin;
        newTrash->head       = cell;
        newTrash->nextTrash  = garbageCollectorHead;
        garbageCollectorHead = newTrash;
        garbageCollector     = garbageCollectorHead;
    }
}

void Player::Impl::destroyTrash( Blist trashHead )
{
    if( trashHead )
    {
        destroyTrash( trashHead->next );
        delete trashHead;
    }
}

void Player::Impl::destroyBin( Pbin bin )
{
    if( bin )
    {
        if( bin->head )
            destroyTrash( bin->head );
        destroyBin( bin->nextTrash );
        delete bin;
    }
}
