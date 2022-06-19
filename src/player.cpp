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
struct BoardCell;
typedef BoardCell* BoardList;

struct Bin
{
    BoardList head;
    Bin* nextTrash;
};
typedef Bin* Pbin;

// Class Pawn definition
class Pawn
{
    public:
        Pawn();
        Pawn( const Pawn& );
        Pawn( const char&, const short&, const short&, const short& );
        void setValues( const char&, const short&, const short& );
        void setCoordinates( const short&, const short& );
        short  getCol() const;
        short  getRow() const;
        bool isQueen() const;
        char getType() const;
        bool checkValidCoordinates( short, short );
        void setQueen();
        short  getPlayer() const;
        void deletePawn();
        Pawn operator = ( const Pawn& );
        bool operator == ( const Pawn& );

    private:
        bool queen;
        char type;
        short  player;
        short  x;
        short  y;
};
typedef Pawn* Pawn_t;

// Class Board definition
class Board
{
    public:
        Board();
        Board( char b[ ROWS ][ COLS ] );
        Board( Board& );
        Board& operator = ( const Board& );
        void   printBoard();
        Pawn&  getPawn( short, short );
        void   move( Pawn& ,short, short );
        void   getAllMoves(short);
        void   getValidMoves( Pawn& );
        void   updatePieceLeft();
        void   traverseLeft ( short, short, short, const Pawn&, short, std::pair< short, short> );
        void   traverseRight( short, short, short, const Pawn&, short, std::pair< short, short> );
        void   destroy( BoardList& ) const;
        void   prepend( BoardList& cell, const Board& moved );
        void   append( BoardList& cell, const Board& moved);
        short  boardDifference();
        short  wins();
        short  getXLeft();
        short  getOLeft();
        short  getXKing();
        short  getOKing();
        float  evalutation();
        float  getScore();
        void   setScore(float);
        short  countDamaSpot(short);
        BoardList head;

    private:
        short xLeft;
        short oLeft;
        short xKing;
        short oKing;
        float eval;
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

struct BoardCell
{
    Board movedBoard;
    BoardList nextMove;
    void append(BoardList);
};

// Impl Definition
struct Player::Impl
{
    short player_num;

    Pcell head;
    Pcell tail;

    Pbin garbageCollector;
    Pbin garbageCollectorHead;
    Pbin garbageCollectorTail;

    void  destroy( Pcell& ) const;
    void   append( Pcell );
    void   printBoard( const Pcell& );
    void   printMemBoard( const Pcell& );
    void   listBoards();
    void   listRevBoards();
    Pcell  copy( Pcell& );
    piece  findEnum( const char& );
    Pcell  bringPcell( const int& );
    Pboard minimax( const Pboard&, short, bool, float, float );
    bool   validBoard( Pcell& );
    void   appendToBin( BoardList& );
    void   destroyBin( Pbin );
    void   destroyTrash( BoardList );
};

struct Prediction
{
   Pawn before;
   Pawn after;
};

/// Start of Pawn class implementation ////////////////////////////////////////

Pawn::Pawn()
{
    x     = 0;
    y     = 0;
    type  = 'e';
    queen = false;
}

Pawn::Pawn( const char& t, const short& row, const short& col, const short& playerNr )
{
    type   = t;
    x      = col;
    y      = row;
    player = playerNr;
    if(type == 'X' || type == 'O')
        queen = true;
}

Pawn::Pawn( const Pawn& p )
{
    x      = p.x;
    y      = p.y;
    type   = p.type;
    player = p.player;
    queen  = p.queen;
}

void Pawn::setValues( const char& t, const short& row, const short& col)
{
    type = t;
    setCoordinates(row, col);
    if(type == 'x' || type == 'X')
        player = 1;
    else if(type == 'o' || type == 'O')
        player = 2;
    else
        player = 0;
    if(type == 'X' || type == 'O')
        queen = true;
    else
        queen = false;
}

void Pawn::deletePawn()
{
    type   = 'e';
    player =  0;
    queen  = false;
}

void Pawn::setQueen()
{
    if(queen == false)
    {
        queen = true;
        type == 'x' ? type = 'X': type = 'O';
    }
}

void Pawn::setCoordinates( const short& row, const short& col)
{
    if( checkValidCoordinates(row, col) )
    {
        x = col;
        y = row;
    }
}

bool Pawn::checkValidCoordinates(short row, short col)
{
    if(row < 0 || row > 7 || col < 0 || col > 14)
        exit (1);
    return true;
}

bool Pawn::isQueen() const
{
    if( queen )
        return true;
    return false;
}

short Pawn::getCol() const
{
    return x;
}

short Pawn::getRow() const
{
    return y;
}

char Pawn::getType() const
{
    return type;
}

short Pawn::getPlayer() const
{
    return player;
}

Pawn Pawn::operator = ( const Pawn& p )
{
    type   = p.type;
    queen  = p.queen;
    player = p.player;
    return *this;
}

bool Pawn::operator == ( const Pawn& p )
{
    if( this->getType() == p.getType() &&
        this->getRow() == p.getRow() &&
        this->getCol() == p.getCol() &&
        this->getPlayer() == p.getPlayer()
      )
        return true;
    return false;
}
/// End of Pawn class implementation //////////////////////////////////////////

/// Start Board class implementation //////////////////////////////////////////
///
Board::Board()
{
    for( short i = ( ROWS - 1 ); i >= 0; --i )
        for( short j = 0; j < COLS; ++j )
            board[ i ][ j ].setValues( 'e', i, j );
    updatePieceLeft();
    head = nullptr;
    eval = evalutation();
}

Board::Board( char b[ROWS][COLS] )
{
    for( short i = ( ROWS - 1 ); i >= 0; --i )
        for( short j = 0; j < COLS; ++j )
            board[ i ][ j ].setValues( b[ i ][ j ], i, j );
    updatePieceLeft();
    head = nullptr;
    eval = evalutation();
}

Board::Board( Board& source )
{
    for( short i = ( ROWS - 1 ); i >= 0; --i )
        for( short j = 0; j < COLS; ++j )
            board[ i ][ j ].setValues(source.getPawn(i, j).getType(), i, j );
    updatePieceLeft();
    head = nullptr;
    eval = evalutation();
}

Board& Board::operator=( const Board& b )
{
    for(short i(ROWS - 1); i >= 0; --i)
        for(short j(0); j < COLS; ++j)
            board[i][j].setValues(b.board[i][j].getType(), i, j);
    eval  = b.eval;
    xLeft = b.xLeft;
    oLeft = b.oLeft;
    xKing = b.xKing;
    oKing = b.oKing;
    return *this;
}

Pawn& Board::getPawn( short row, short col )
{
    return board[ row ][ col ];
}

void Board::printBoard()
{
    cout << "---------------"<< endl;
    for( short i = ( ROWS - 1 ); i >= 0; --i )
    {
         for( short j = 0; j < COLS; ++j )
            board[ i ][ j ].getType() == 'e' ? cout << ' ' :  cout << board[ i ][ j ].getType();
         cout << endl;
    }
    cout << "---------------\n"<< endl;
}

void Board::move(Pawn& piece,short row, short col)
{
    Pawn support(board[row][col]);
    board[row][col] = piece;
    piece = support;
    if( row == (ROWS - 1) || row == 0 )
        board[row][col].setQueen();
}

short Board::countDamaSpot(short num)
{
    short res = 0;
    int row;
    char type;
    num == 1 ? row = 7 : row = 0;
    row == 0 ? type = 'O' : type = 'X';
    for(int i = 0; i < COLS; i++)
        if(board[row][i].getType() == type)
            ++res;
    return res;
}

void Board::updatePieceLeft()
{
    xLeft = xKing = 0;
    oLeft = oKing = 0;
    for( short i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( short j = 0; j < COLS; ++j )
        {
            switch ( board[ i ][ j ].getType())
            {
                case 'x':
                     ++xLeft;
                     break;
                case 'o':
                    ++oLeft;
                    break;
                case 'X':
                    ++xKing;
                    break;
                case 'O':
                    ++oKing;
                    break;
                case 'e':
                    break;
                default:
                    throw player_exception { player_exception::missing_file, "Invalid character in board loaded!!..." };
            }
        }
    }
    if( ( ( xLeft + xKing ) + ( oLeft + oKing ) ) > MPED )
        throw player_exception { player_exception::invalid_board, "To many pieces in the loaded board!!..." };
}

void Board::prepend( BoardList& cell, const Board& moved )
{
    BoardList newOne = new BoardCell;
    newOne->movedBoard = moved;
    newOne->nextMove = cell;
    cell = newOne;
}

void Board::append( BoardList& cell, const Board& moved )
{
    if( cell == nullptr )
        prepend( cell, moved );
    else
        append( cell->nextMove, moved );
}

short Board::getOKing()
{
    return oKing;
}

short Board::getXKing()
{
    return xKing;
}

short Board::getXLeft()
{
    return xLeft + xKing;
}


short Board::getOLeft()
{
    return oLeft + oKing;
}

short Board::wins()
{
    if( ( oLeft + oKing ) == 0 )
        return 1;
    else if( ( xLeft + xKing ) == 0 )
        return 2;
    return 0;
}

float Board::evalutation()
{
  float mult = 0.5f;
  float res = (  ( float )xLeft - ( float )oLeft + ( oKing * mult - xKing * mult ) );
  return res;
}

void Board::setScore( float n )
{
    eval = n;
}

void Board::traverseRight( short start, short stop, short step, const Pawn& pawn, short right,  std::pair< short, short > lastPos )
{
    bool eatable = false;
    bool moved   = false;
    bool blocked = false;

    for( short row = start; row < ROWS && row != stop && !moved; row = (short)(row + step) )
    {
        if( right >= COLS )
            break;
        Pawn current = getPawn( row, right );
        if( current.getType() == 'e' && !blocked )
        {
            Board choice = *this;
            choice.move( choice.getPawn( pawn.getRow(), pawn.getCol()), current.getRow(), current.getCol() );
            if( eatable )
            {
                choice.getPawn( lastPos.first, lastPos.second ).deletePawn();
                choice.updatePieceLeft();
            }
            append( head, choice );
            moved = true;
        }
        else if( current.getPlayer() == pawn.getPlayer() || ( current.isQueen() && !pawn.isQueen() ) )
            blocked = true;
        else
        {
            eatable = true;
            lastPos.first = current.getRow();
            lastPos.second = current.getCol();
        }
        right = (short)(right + 2);
    }
}

void Board::traverseLeft( short start, short stop, short step, const Pawn& pawn, short left, std::pair< short, short > lastPos )
{
    bool eatable = false;
    bool moved   = false;
    bool blocked = false;
    for( short row = start; row < ROWS && row != stop && !moved; row = (short)( row + step ) )
    {
        if( left < 0 )
            break;
        Pawn current = getPawn( row, left );
        if( current.getType() == 'e' && !blocked )
        {
            Board choice = *this;
            choice.move( choice.getPawn( pawn.getRow(), pawn.getCol() ), current.getRow(), current.getCol() );
            if( eatable )
            {
                choice.getPawn( lastPos.first, lastPos.second ).deletePawn();
                choice.updatePieceLeft();
            }
            append( head, choice );
            moved = true ;
        }
        else if( ( current.getPlayer() == pawn.getPlayer() ) || ( current.isQueen() && !pawn.isQueen() ) )
            blocked = true;
        else
        {
            eatable = true;
            lastPos.first = current.getRow();
            lastPos.second = current.getCol();
        }
        left = ( short )( left - 2 );
    }
}

void Board::getValidMoves( Pawn& p )
{
    short   left  = (short)(p.getCol() - 2);
    short   right = (short)(p.getCol() + 2);
    short   row   = p.getRow();
    if( p.getType() == 'x' || p.isQueen() )
    {
        traverseLeft ( (short)(row + 1), (short)( std::max( row + 3, -1 ) ), 1, p, left,  std::pair<short, short>( 0, 0 ) );
        traverseRight( (short)(row + 1), (short)( std::max( row + 3, -1 ) ), 1, p, right, std::pair<short, short>( 0, 0 ) );
    }
    if( p.getType() == 'o' || p.isQueen() )
    {
        traverseLeft ( (short)(row - 1), (short)(std::max( row - 3, -1 ) ), -1, p, left,  std::pair<short, short>( 0, 0 ) );
        traverseRight( (short)(row - 1), (short)(std::max( row - 3, -1 ) ), -1, p, right, std::pair<short, short>( 0, 0 ) );
    }
}

void Board::getAllMoves( short player )
{
    for(short i(ROWS - 1); i >= 0; --i)
        for(short j(0); j < COLS; ++j)
            if( board[i][j].getPlayer() == player )
                getValidMoves( board[ i ][ j ] );
}

float Board::getScore()
{
    return eval;
}

void Board::destroy( BoardList& cell ) const
{
    if(cell)
    {
        destroy(cell->nextMove);
        delete cell;
    }
}


/// End Board class implementation ////////////////////////////////////////////

/// Player class implementation ///////////////////////////////////////////////

Player::Player( int player_nr )
{
    if( player_nr != 1 && player_nr != 2 ) throw player_exception{ player_exception::index_out_of_bounds, "player_nr is neither player1 or player2 in Player constructor!!" };
    pimpl                       = new Impl;
    pimpl->player_num           = (short) player_nr;
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
    newPcell->b.updatePieceLeft();
    if(!pimpl->validBoard( newPcell ) || newPcell->b.getXLeft() > 12 || newPcell->b.getOLeft() > 12 ) throw player_exception { player_exception::invalid_board, "Invalid Board in load_board() func..." };
    pimpl->append( newPcell );
    valid_move() == true ? cout << "MOSSA VALIDA\n" : cout << "MOSSA NON VALIDA\n";
}

bool Player::valid_move() const
{
    if(!pimpl->tail->prev) throw player_exception { player_exception::index_out_of_bounds, "Too few boards in History in valid_move() func..." };

    // Caso in cui non ci siano nuove dame, controllo solo il percorso delle pedine
    short diff = 0;
    Prediction difference[4];
    for(int i = 0; i < ROWS && diff < 4; i++)
    {
        for(int j = 0; j < COLS; j++)
        {            // se il pezzo in (i, j) nella board attuale != dal pezzo in (i, j) nella board precedente
            if(pimpl->tail->b.getPawn(i, j).getType() != pimpl->tail->prev->b.getPawn(i, j).getType())
            {
                difference[diff].before = pimpl->tail->prev->b.getPawn(i, j);
                difference[diff].before.setCoordinates(i, j);
                difference[diff].after = pimpl->tail->b.getPawn(i, j);
                difference[diff++].after.setCoordinates(i,j);
            }
        }
    }
    if(diff < 2 || diff > 3)
          return false;

    // cout << "Parola di cristo" << endl;
    // for(int i = 0; i < 4; i++)
    //     cout << "B: " << difference[i].before.getType() << "\tX: " << difference[i].before.getCol() << "\tY: " << difference[i].before.getRow()
    //          << "\nA: " << difference[i].after.getType() << "\tX: " << difference[i].after.getCol() << "\tY: " << difference[i].after.getRow()
    //          << endl;

    // Dentro il nostro array abbiamo tutte le differenze nelle due ultime boards
    // Trovo il/i pezzo/i che si sono mossi -> escludo le 'e'
    // lancio getValidMoves moves sul pezzo della board precedente e verifico la presenza della
    // mossa che risiede in after della board successiva
    if(diff == 2)
    {
        Pawn p { difference[0].before.getType() != 'e' ? difference[0].before : difference[1].before };
        Pawn x { difference[1].after.getType() != 'e' ? difference[1].after : difference[0].after };
        // cout << "PREV " << p.getType() << '\t' << p.getRow() << ',' << p.getCol() << endl;
        // cout << "NEXT " << x.getType() << '\t' << x.getRow() << ',' << x.getCol() << endl;
        Pcell pc { pimpl->tail->prev };
        pc->b.getValidMoves(p);
        auto moveMe = pc->b.head;
        while(moveMe != nullptr)
        {
            if(moveMe->movedBoard.getPawn(x.getRow(), x.getCol()) == x)
            {
                pc->b.destroy(pc->b.head);
                return true;
            }
            moveMe = moveMe->nextMove;
        }
        pc->b.destroy(pc->b.head);
        return false;
    }
    else
    {
        Pawn p { difference[0].before.getType() != 'e' ? difference[0].before : difference[2].before };
        Pawn x { difference[2].after.getType() != 'e' ? difference[2].after : difference[0].after };
        // cout << "PREV " << p.getType() << '\t' << p.getRow() << ',' << p.getCol() << endl;
        // cout << "NEXT " << x.getType() << '\t' << x.getRow() << ',' << x.getCol() << endl;
        Pcell pc { pimpl->tail->prev };
        pc->b.getValidMoves(p);
        auto moveMe = pc->b.head;
        while(moveMe != nullptr)
        {
            if(moveMe->movedBoard.getPawn(x.getRow(), x.getCol()) == x)
            {
                pc->b.destroy(pc->b.head);
                return true;
            }
            moveMe = moveMe->nextMove;
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
    if( pimpl->player_num == 1 )
       bestMove =  pimpl->minimax( &pimpl->tail->b, 4, true, MINF, PINF );
    else
        bestMove = pimpl->minimax( &pimpl->tail->b, 4, false, MINF, PINF );
    bestMove->printBoard();
    Pcell ciao = new Cell;
    ciao->b = *bestMove;
    pimpl->validBoard( ciao );
    pimpl->append( ciao );
    if( pimpl->garbageCollector != nullptr )
        pimpl->destroyBin(pimpl->garbageCollector);
}

void Player::store_board( const string &filename, int history_offset ) const
{
    Pcell moveMe = pimpl->bringPcell( history_offset );
    ofstream outputFile;
    outputFile.open( filename );
    for( short i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( short j = 0; j < COLS; ++j )
            moveMe->b.getPawn(i, j).getType() == 'e' ? outputFile << ' ' : outputFile << moveMe->b.getPawn(i, j).getType();
        if( i > 0 )
            outputFile << endl;
    }
    outputFile.close();
}

Player::piece Player::operator()( int r, int c, int history_offset ) const
{
    if( ( r < 0 || r > 7 ) || ( c < 0 || c > 14) ) throw player_exception { player_exception::index_out_of_bounds, "Wrong coordinates... out of range in operator() func..." };
    Pcell moveMe = pimpl->bringPcell( history_offset );
    return pimpl->findEnum( moveMe->b.getPawn( (short)r, (short)c ).getType());
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
                if( reference->b.getPawn(i, j).getType() != moveMe->b.getPawn(i, j).getType())
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
    return wins(pimpl->player_num);
}

bool Player::loses( int player_nr ) const
{
    if(player_nr == 1)
        return wins(2);
    return wins(1);
}

bool Player::loses() const
{
    return loses(pimpl->player_num);
}

/// End of Player Implementation //////////////////////////////////////////////
///
/// Impl struct implementation ////////////////////////////////////////////////

Player::piece Player::Impl::findEnum( const char& c )
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

bool Player::Impl::validBoard( Pcell& node )
{
    for( short i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( short j = 0; j < COLS; ++j )
        {
            if( ( i % 2 == 1 ) && ( j % 4 != 0 ) && node->b.getPawn(i, j).getType() != 'e' )
                throw player_exception { player_exception::missing_file, "Invalid piece position in board loaded!!..." };
            else if( ( i % 2 == 0 ) && ( (j - 2) % 4 != 0 ) && node->b.getPawn(i, j).getType() != 'e' )
                throw player_exception { player_exception::missing_file, "Invalid piece position in board loaded!!..." };
        }
    }
    return true;
}

void Player::Impl::printBoard( const Pcell& printCell )
{
    cout << "---------------"<< endl;
    for( short i = ( ROWS - 1 ); i >= 0; --i )
    {
         for( short j = 0; j < COLS; ++j )
            printCell->b.getPawn(i, j).getType() == 'e' ? cout << ' ' : cout <<  printCell->b.getPawn(i, j).getType();
         cout << endl;
    }
    cout << "---------------\n"<< endl;
    printMemBoard( printCell );
}

void Player::Impl::printMemBoard( const Pcell& printCell )
{
    cout << "---------------"<< endl;
    for( short i = (ROWS - 1); i >= 0; --i )
    {
        for( short j = 0; j < COLS; ++j )
           cout <<  printCell->b.getPawn(i, j).getType();

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
        for( Pcell moveMe = head; moveMe != nullptr; moveMe = moveMe->next )
            printBoard( moveMe );
    }
}

void Player::Impl::listRevBoards()
{
    if( head == nullptr )
        cout << "Sorry no boards loaded yet..." << endl;
    else
    {
        for( Pcell moveMe = tail; moveMe != nullptr; moveMe = moveMe->prev )
            printBoard( moveMe );
    }
}

Pcell Player::Impl::bringPcell( const int& history_offset )
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
        BoardList bestMove(nullptr);
        base->getAllMoves(1);
        BoardList move = base->head;
        appendToBin(base->head);
        for( ; move != nullptr; move = move->nextMove )
        {
            eval = minimax( &move->movedBoard, (short)( depth - 1 ), false, alpha, beta );
            if( eval )
            {
                maxEval = std::max( maxEval, eval->getScore() );
                alpha = std::max( alpha, maxEval );
                if (beta <= alpha)
                    break;
                if( maxEval == eval->getScore() )
                {
                    bestMove = move;
                    bestMove->movedBoard.setScore(eval->getScore());
                }
            }
        }
        if( bestMove == nullptr )
            return base;
        return &bestMove->movedBoard;
    }
    else
    {
        Pboard eval( nullptr );
        BoardList bestMove( nullptr );
        float minEval = PINF;
        base->getAllMoves(2);
        BoardList move = base->head;
        appendToBin(base->head);
        for(;move != nullptr; move = move->nextMove)
        {
            eval = minimax( &move->movedBoard, (short )( depth - 1 ), true, alpha, beta );
            if( eval )
            {
                minEval = std::min( minEval, eval->getScore() );
                beta = std::min( beta, minEval );
                if( beta <= alpha )
                    break;
                if( minEval == eval->getScore())
                {
                    bestMove = move;
                    bestMove->movedBoard.setScore(eval->getScore());
                }
            }
        }
        if( bestMove == nullptr )
            return base;
        return &bestMove->movedBoard;
    }
}

/// End of Impl implementation ////////////////////////////////////////////////
void Player::Impl::appendToBin( BoardList& cell )
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

void Player::Impl::destroyTrash( BoardList trashHead )
{
    if( trashHead )
    {
        destroyTrash( trashHead->nextMove );
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
