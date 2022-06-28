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

struct Pair
{
    short x;
    short y;
};

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

        void   diagonals( short, short, short, const Pawn&, short, bool, Pair );

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
    Board board;
    Cell* next;
    Cell* prev;
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
    Bin* next;

    void destroy( Blist& ) const;
    void prepend( Pbin&, const Blist& );
    void append( const Pbin&, const Blist& );
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

    Pcell  search_history( const int& );

    bool verify_board( Pcell& );

    // Minimax Algo
    Pboard minimax( const Pboard&, short, bool, float, float );

    // Helper list for minimax algo
    Pbin pointers_list;

    // Utility funcs for Helper list
    void collect_pointer( Blist& );
    void remove_pointers( Pbin );
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
    x      = copy_pawn.x;
    y      = copy_pawn.y;
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

short Board::wins()
{
    short win( 0 );
    short opieces = (short)( oleft + oqueen );
    short xpieces = (short)( xleft + xqueen );

    if( opieces && xpieces )
      return win;
    xleft == 0 ? win = 2 : win = 1;
    return win;
}

float Board::eval_board()
{
    float res( 0 );
    float mult = 0.5f;
    selected_player == 1 ? res = (  ( float )xleft - ( float )oleft + (  xqueen * mult - oqueen * mult ) ) : res = (  ( float )oleft - ( float )xleft + (  oqueen * mult - xqueen * mult ) );
    return res;
}

void Board::set_score( float num )
{
    score = num;
}

void Board::diagonals( short start, short stop, short step, const Pawn& selected_pawn, short dir, bool is_right, Pair prev_coordinates )
{
    bool is_eatable( false );
    bool pawn_moved( false );
    bool is_movable( true );

    short row( start );
    Pawn current;
    Board choice;
    short inc( 0 );
    is_right ? inc = 2 : inc = ( -2 );

    while( row < ROWS && row != stop && !pawn_moved )
    {
        if( ( is_right && dir >= COLS ) || ( !is_right && dir < 0 ) )
            break;
        current = this->at( row, dir );

        if( ( current.is_queen() && selected_pawn.is_queen() ) || ( current.get_player() == selected_pawn.get_player() ) )
        {
            is_movable = false;
        }
        if( current.get_type() == 'e' && is_movable )
        {
            choice = *this;
            choice.move_piece( choice.at( selected_pawn.get_row(), selected_pawn.get_col() ), current.get_row(), current.get_col() );
            if( is_eatable )
            {
                choice.at( prev_coordinates.y, prev_coordinates.x ).del_pawn();
                choice.update_pieces();
            }
            append( head, choice );
            pawn_moved = true;
        }
        else
        {
            is_eatable = true;
            prev_coordinates.y = current.get_row();
            prev_coordinates.x = current.get_col();
        }

        dir += inc;
        row += step;
    }
}

void Board::get_valid_moves( Pawn& p )
{
    short   left_side  = (short)(p.get_col() - 2);
    short   right_side = (short)(p.get_col() + 2);
    short   row   = p.get_row();
    Pair    pos{ 0, 0 };
    if( p.get_type() == 'x' || p.is_queen() )
    {
        // left
        diagonals( (short)(row + 1), (short)( std::max( row + 3, -1 ) ), 1, p, left_side, false,  pos );
        // right
        diagonals( (short)(row + 1), (short)( std::max( row + 3, -1 ) ), 1, p, right_side, true, pos );
    }
    if( p.get_type() == 'o' || p.is_queen() )
    {
        // left
        diagonals( (short)(row - 1), (short)(std::max( row - 3, -1 ) ), -1, p, left_side, false,  pos );
        // right
        diagonals( (short)(row - 1), (short)(std::max( row - 3, -1 ) ), -1, p, right_side, true, pos );
    }
}

void Board::get_all_moves( short owner )
{
    short i( 0 );
    short j( 0 );
    while( i < ROWS )
    {
        while( j < COLS)
        {
            if( owner == this->at( i, j ).get_player() )
                get_valid_moves( this->at( i, j ) );
            j++;
        }
        j = 0;
        i++;
    }
}

float Board::get_score()
{
    return score;
}

void Board::destroy( Blist& board_ptr ) const
{
    if( board_ptr )
    {
        destroy( board_ptr->next );
        delete board_ptr;
    }
}

Player::Player( int player_nr )
{
    if( player_nr != 1 && player_nr != 2 ) throw player_exception{ player_exception::index_out_of_bounds, "player_nr is neither player1 or player2 in Player constructor!!" };

    pimpl           = new Impl;
    pimpl->head     = nullptr;
    pimpl->tail     = nullptr;
    pimpl->num      = (short) player_nr;
    selected_player = pimpl->num;

    pimpl->pointers_list     = nullptr;
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

Player& Player::operator = ( const Player& player )
{
    if( this != &player )
    {
        pimpl = new Impl;
        pimpl->destroy( pimpl->head );

        Pcell move_me = player.pimpl->head;
        while( move_me != nullptr )
        {
            pimpl->append( pimpl->copy( move_me ) );
            move_me = move_me->next;
        }
    }

    return *this;
}

void Player::init_board( const string& filename ) const
{
    ofstream initFile;
    initFile.open( filename );

    for( short rows( 0 ); ( initFile.good() ) && ( rows < ROWS ); rows++ )
    {
        for( short cols ( 0 ); cols < COLS; cols++ )
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


void Player::load_board( const string& filename )
{
    ifstream loadFile;
    loadFile.open( filename );
    if( !loadFile.is_open() ) throw player_exception{ player_exception::missing_file, "Missing file in load_board() func!!..." };

    char char_board[ROWS][COLS];
    string str;
    Pcell cell_ptr = new Cell;

    short i ( ROWS - 1 );

    while( getline( loadFile, str ) )
    {
        if( str.length() != 15 ) throw player_exception { player_exception::invalid_board, "Invalid length in load_board() func..." };
        for( size_t j( 0 ); j < COLS; j++ )
        {
            if( (char)str.at( j ) == ' ')
                str.at( j ) = 'e';
            char_board[ i ][ j ] = (char)str.at( j );
        }
        i--;
    }
    loadFile.close();

    Board new_board( char_board );

    cell_ptr->board = new_board;
    cell_ptr->board.update_pieces();

    if( !pimpl->verify_board( cell_ptr ) || ( cell_ptr->board.get_oleft() > 12 ) || ( cell_ptr->board.get_xleft() > 12 )  ) throw player_exception { player_exception::invalid_board, "Invalid Board in load_board() func..." };
    pimpl->append( cell_ptr );
}

bool Player::valid_move() const
{
if(!pimpl->tail->prev) throw player_exception { player_exception::index_out_of_bounds, "Too few boards in History in valid_move() func..." };

    // Catch dama
    short history_xqueen = pimpl->tail->prev->board.dama_counter(1);
    short current_xqueen = pimpl->tail->board.dama_counter(1);
    short history_oqueen = pimpl->tail->prev->board.dama_counter(2);
    short current_oqeen = pimpl->tail->board.dama_counter(2);

    if(history_xqueen == -1 || current_xqueen == -1 || history_oqueen == -1 || current_oqeen == -1)
        return false;
    if(current_xqueen - history_xqueen > 1)
        return false;
    if(current_oqeen - history_oqueen > 1)
        return false;

    short diff = 0;
    Prediction difference[4];
    for(short i = 0; i < ROWS && diff < 4; i++)
    {
        for(short j = 0; j < COLS; j++)
        {
            if(pimpl->tail->board.at(i, j).get_type() != pimpl->tail->prev->board.at(i, j).get_type())
            {
                difference[diff].before = pimpl->tail->prev->board.at(i, j);
                difference[diff].before.set_coordinates(i, j);
                difference[diff].after = pimpl->tail->board.at(i, j);
                difference[diff++].after.set_coordinates(i,j);
            }
        }
    }
    if(diff < 2 || diff > 3)
        return false;

    Pcell cell_ptr { pimpl->tail->prev };
    Blist move_me  = nullptr;
    cell_ptr->board.head = nullptr;

    if( diff == 2 )
    {
        Pawn history_pawn { difference[0].before.get_type() != 'e' ? difference[0].before : difference[1].before };
        Pawn current_pawn { difference[1].after.get_type()  != 'e' ? difference[1].after : difference[0].after };

        cell_ptr->board.get_valid_moves( history_pawn );
        move_me = cell_ptr->board.head;
        while( move_me )
        {
            if( move_me->current.at( current_pawn.get_row(), current_pawn.get_col() ) == current_pawn )
            {
                cell_ptr->board.destroy( cell_ptr->board.head );
                return true;
            }
            move_me = move_me->next;
        }
        cell_ptr->board.destroy( cell_ptr->board.head );
        return false;
    }
    else
    {
        Pawn history_pawn { difference[0].before.get_type() != 'e' ? difference[0].before : difference[2].before };
        Pawn current_pawn { difference[2].after.get_type()  != 'e' ? difference[2].after : difference[0].after };

        cell_ptr->board.get_valid_moves( history_pawn );
        move_me = cell_ptr->board.head;
        while( move_me )
        {
            if( move_me->current.at( current_pawn.get_row(), current_pawn.get_col() ) == current_pawn )
            {
                cell_ptr->board.destroy(cell_ptr->board.head);
                return true;
            }
            move_me = move_me->next;
        }
        cell_ptr->board.destroy(cell_ptr->board.head);
        return false;
    }
    return false;
}

void Player::move()
{
    if( !pimpl->head ) throw player_exception { player_exception::index_out_of_bounds, "Empty History in move() func..." };

    Pboard res_move( nullptr );
    pimpl->pointers_list = nullptr;

    res_move = pimpl->minimax( &pimpl->tail->board, 4, true, MINF, PINF );

    // DELETE ME
    res_move->print_board();

    Pcell new_cell = new Cell;
    new_cell->board = *res_move;
    pimpl->verify_board( new_cell );
    pimpl->append( new_cell );

    if( pimpl->pointers_list )
    {
        pimpl->remove_pointers( pimpl->pointers_list );
    }
}

void Player::store_board( const string &filename, int history_offset ) const
{
    Pcell move_me( pimpl->search_history( history_offset ) );
    ofstream output_file;
    output_file.open( filename );

    short i( ROWS - 1 );
    short j( 0 );
    while( i >= 0 )
    {
        while( j < COLS )
        {
            move_me->board.at(i, j).get_type() == 'e' ? output_file << ' ' : output_file << move_me->board.at(i, j).get_type();
            j++;
        }
        if( i > 0 )
        {
            output_file << endl;
        }
        
        j = 0;
        i--;
    }
    output_file.close();
}

Player::piece Player::operator()( int r, int c, int history_offset ) const
{
    if( ( r < 0 || r > 7 ) || ( c < 0 || c > 14) ) throw player_exception { player_exception::index_out_of_bounds, "Wrong coordinates... out of range in operator() func..." };
    Pcell move_me = pimpl->search_history( history_offset );
    return pimpl->select_enum( move_me->board.at( (short)r, (short)c ).get_type() );
}

void Player::pop()
{
    if( !pimpl->head ) throw player_exception { player_exception::index_out_of_bounds, "Empty History in pop() func..."};

    if( pimpl->tail->prev )
    {
        Pcell temp_tail = pimpl->tail->prev;
        pimpl->destroy( pimpl->tail );

        pimpl->tail     = temp_tail;
        temp_tail->next = nullptr;
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

    bool  is_different;
    short counter   = 1;
    Pcell reference = pimpl->tail;
    Pcell move_me    = pimpl->tail->prev;

    short i( ROWS - 1 );
    short j( 0 );
    while( move_me )
    {
        is_different = false;
        while( i >= 0 && !is_different )
        {
            while( j < COLS && !is_different )
            {
                if( move_me->board.at(i, j).get_type() != reference->board.at(i, j).get_type())
                    is_different = true;
                j++;
            }
            j = 0;
            i--;
        }
        if( !is_different )
            counter++;
        i = ( ROWS - 1 );
        move_me = move_me->prev;
    }

    return counter;
}

bool Player::wins( int player_nr ) const
{
    if( pimpl->tail == nullptr ) throw player_exception { player_exception::index_out_of_bounds, "Empty history in wins() or loses() func..." };
    if( pimpl->tail->board.wins() == player_nr )
        return true;
    return false;
}

bool Player::wins() const
{
    return wins( pimpl->num );
}

bool Player::loses( int player_nr ) const
{
    short choice = 0;

    player_nr == 1 ? choice = 2 : choice = 1;
    return wins( choice );
}

bool Player::loses() const
{
    return loses( pimpl->num );
}

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
    short i( ROWS - 1 );
    short j( 0 );
    while( i >= 0 )
    {
        while( j < COLS )
        {
            if( ( i % 2 == 1 ) && ( j % 4 != 0 ) && node->board.at(i, j).get_type() != 'e' )
                throw player_exception { player_exception::missing_file, "Invalid piece position in board loaded!!..." };
            else if( ( i % 2 == 0 ) && ( (j - 2) % 4 != 0 ) && node->board.at(i, j).get_type() != 'e' )
                throw player_exception { player_exception::missing_file, "Invalid piece position in board loaded!!..." };
            j++;
        }
        j = 0;
        i--;
    }
    return true;
}

// DELETE ME
void Player::Impl::print_board( const Pcell& printCell )
{
    cout << "---------------"<< endl;
    for( short i = ( ROWS - 1 ); i >= 0; --i )
    {
         for( short j = 0; j < COLS; ++j )
            printCell->board.at(i, j).get_type() == 'e' ? cout << ' ' : cout <<  printCell->board.at(i, j).get_type();
         cout << endl;
    }
    cout << "---------------\n"<< endl;
}

Pcell Player::Impl::search_history( const int& history_offset )
{
    Pcell move_me = this->tail;
    short history( 0 );

    while( ( history != history_offset ) && ( move_me->prev != nullptr ) )
    {
        move_me = move_me->prev;
        history++;
    }
    if( history_offset > history ) throw player_exception { player_exception::index_out_of_bounds, "History offset greater than history size!..." };
    return move_me;
}

void Player::Impl::destroy( Pcell& node ) const
{
    if( node )
    {
        destroy( node->next );
        delete node;
    }
}

void Player::Impl::append( Pcell cell_ptr )
{
    if( head == nullptr )
    {
        head           = cell_ptr;
        tail           = cell_ptr;
        cell_ptr->prev = nullptr;
    }
    else
    {
        tail->next     = cell_ptr;
        cell_ptr->prev = tail;
        tail           = cell_ptr;
    }
    cell_ptr->next = nullptr;
}

Pcell Player::Impl::copy( Pcell& copy_node )
{
    Pcell cell_ptr  = new Cell;
    cell_ptr->board = copy_node->board;

    return cell_ptr;
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
        collect_pointer(base->head);
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
        collect_pointer(base->head);
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

void Bin::destroy( Blist& board_ptr ) const
{
    if( board_ptr )
    {
        destroy( board_ptr->next );
        delete board_ptr;
    }
}

void Bin::prepend( Pbin& list_head, const Blist& board_ptr )
{
    Pbin new_bptr  = new Bin;
    new_bptr->head = board_ptr;
    new_bptr->next = nullptr;
    list_head      = new_bptr;
}

void Bin::append( const Pbin& list_head, const Blist& board_ptr )
{
    Pbin new_bptr  = new Bin;
    new_bptr->head = board_ptr;
    new_bptr->next = nullptr;

    Pbin move_me { list_head };
    while( move_me->next )
    {
        move_me = move_me->next;
    }
    move_me->next = new_bptr;
}

void Player::Impl::collect_pointer( Blist& board_ptr )
{
    if( !pointers_list )
    {
        pointers_list->prepend( pointers_list, board_ptr );
    }
    else
    {
        pointers_list->append( pointers_list, board_ptr );
    }
}


void Player::Impl::remove_pointers( Pbin pointer_list )
{
    if( pointer_list )
    {
        if( pointer_list->head )
        {
            pointer_list->destroy( pointer_list->head );
        }

        remove_pointers( pointer_list->next );
        delete pointer_list;
    }
}
