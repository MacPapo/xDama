#include "player.hpp"

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
struct Pair;

// Class Pawn definition
class Pawn
{
    public:
        Pawn();
        Pawn( Pawn& );
        Pawn( char, int, int, int );
        void setValues( char, int, int );
        void setCoordinates( int, int );
        int  getX();
        int  getY();
        bool checkValidCoordinates( int, int );
        bool isQueen();
        void printType();
        char getType();
        void setQueen();
        int  getPlayer();
        void deletePawn();
        Pawn operator = ( Pawn& );

    private:
        char type;
        int  player;
        int  x;
        int  y;
        bool queen;
};
typedef Pawn* Pawn_t;

// Class Board definition
class Board
{
    public:
        Board( char b[ ROWS ][ COLS ] );
        Board( Board& );
        void  initBoard();
        void  setBoard();
        void  getBoard();
        void  printBoard();
        Pawn& getPawn( int, int );
        void  move( Pawn& ,int row, int col );
        Cell* getValidMoves( Pawn& );
        Cell* getAllMoves(int);
        void  updatePieceLeft();
        void  traverseLeft ( int, int, int, Pawn, int, std::pair< int, int> );
        void  traverseRight( int, int, int, Pawn, int, std::pair< int, int> );
        void  destroy( Cell* ) const;
        void  append( Cell*&, int );
        int   wins();
        int   getXLeft();
        int   getOLeft();
        double evalutation();
        void initHead();
        Cell* xHead;
        Cell* oHead;

        Cell* xTail;
        Cell* oTail;

    private:
        Pawn  board[ ROWS ][ COLS ];
        int   xLeft;
        int   oLeft;
        int   xKing;
        int   oKing;
};
typedef Board* Pboard;

// Struct cell for lists
struct Cell
{
    Pboard b;
    struct Cell* next;
    struct Cell* prev;
};
typedef Cell* Pcell;

struct Pair
{
    Pboard pio;
    double score;
};

// Impl Definition
struct Player::Impl
{
    short nPLayer;

    Pcell head;
    Pcell tail;

    void  destroy( Pcell ) const;
    void  append( Pcell );
    void  printBoard( const Pcell& );
    void  printMemBoard( const Pcell& );
    void  listBoards();
    void  listRevBoards();
    Pcell copy( const Pcell& );
    piece findEnum( const char& );
    Pcell bringPcell( const int& );
    Pair  minimax( Pboard&, int, bool );
    bool  validBoard( Pcell& );
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
    char supportBoard[ROWS][COLS];

    string str;
    Pcell newPcell = new Cell;
    int i = ( ROWS - 1 );
    while( getline( loadFile, str ) )
    {
        if( str.length() != 15 ) throw player_exception { player_exception::missing_file, "Invalid length in load_board() func..." };
        for( size_t j = 0; j < COLS; ++j )
        {
            if( (char)str.at( j ) == ' ')
                str.at( j ) = 'e';

            supportBoard[ i ][ j ] = (char)str.at( j );
        }

        --i;
    }
    loadFile.close();

    newPcell->b = new Board(supportBoard);
    newPcell->b->initHead();
    newPcell->b->updatePieceLeft();

    if( pimpl->validBoard( newPcell ) )
        pimpl->append( newPcell );
    else
        delete newPcell->b;
}

void Player::store_board( const string &filename, int history_offset ) const
{
    Pcell moveMe = pimpl->bringPcell( history_offset );

    ofstream outputFile;
    outputFile.open( filename );

    for( int i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( int j = 0; j < COLS; ++j )
            moveMe->b->getPawn(i, j).getType() == 'e' ? outputFile << ' ' : outputFile << moveMe->b->getPawn(i, j).getType();

        if( i > 0 )
            outputFile << endl;
    }
    outputFile.close();
}

Player::piece Player::operator()( int r, int c, int history_offset ) const
{
    if( ( r < 0 || r > 7 ) || ( c < 0 || c > 14) ) throw player_exception { player_exception::index_out_of_bounds, "Wrong coordinates... out of range in operator() func..." };

    Pcell moveMe = pimpl->bringPcell( history_offset );
    return pimpl->findEnum( moveMe->b->getPawn(r, c).getType());
}

void Player::move()
{
    Pair deleteMe;

    if( pimpl->nPLayer == 1 )
        deleteMe = pimpl->minimax( pimpl->tail->b, 1, true );
    else
        deleteMe = pimpl->minimax( pimpl->tail->b, 1, false );

    Pcell ciao = new Cell;
    ciao->b = deleteMe.pio;
    pimpl->validBoard( ciao );
    pimpl->append( ciao );
}

void Player::pop()
{
    if( !pimpl->head ) throw player_exception { player_exception::index_out_of_bounds, "Empty History in pop() func..."};

    if( pimpl->tail->prev )
    {
        Pcell prevTail = pimpl->tail->prev;
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
    Pcell reference = pimpl->tail;
    Pcell moveMe = pimpl->tail->prev;

    while( moveMe != nullptr )
    {
        different = false;
        for( int i = ( ROWS - 1 ); i >= 0 && !different; --i )
            for( int j = 0; j < COLS && !different; ++j )
                if( reference->b->getPawn(i, j).getType() != moveMe->b->getPawn(i, j).getType())
                    different = true;

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

void Player::Impl::destroy( Pcell node ) const
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
        head = newPcell;
        tail = newPcell;
        newPcell->prev = nullptr;
    }
    else
    {
        tail->next = newPcell;
        newPcell->prev = tail;
        tail = newPcell;
    }

    newPcell->next = nullptr;
}

Pcell Player::Impl::copy( const Pcell& node )
{
    Pcell storeMe = new Cell;
    storeMe->b = new Board(*node->b);

    return storeMe;
}

Pcell Player::Impl::bringPcell( const int& history_offset )
{
    int history = 0;
    Pcell moveMe = this->tail;

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

bool Player::Impl::validBoard( Pcell& node )
{
    for( int i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( int j = 0; j < COLS; ++j )
        {
            if( ( i % 2 == 1 ) && ( j % 4 != 0 ) && node->b->getPawn(i, j).getType() != 'e' )
                throw player_exception { player_exception::missing_file, "Invalid piece position in board loaded!!..." };
            else if( ( i % 2 == 0 ) && ( (j - 2) % 4 != 0 ) && node->b->getPawn(i, j).getType() != 'e' )
                throw player_exception { player_exception::missing_file, "Invalid piece position in board loaded!!..." };
        }
    }
    return true;
}

void Player::Impl::printBoard( const Pcell& printPcell )
{
    cout << "---------------"<< endl;
    for( int i = ( ROWS - 1 ); i >= 0; --i )
    {
         for( int j = 0; j < COLS; ++j )
            printPcell->b->getPawn(i, j).getType() == 'e' ? cout << ' ' : cout <<  printPcell->b->getPawn(i, j).getType();

         cout << endl;
    }
    cout << "---------------\n"<< endl;

    printMemBoard( printPcell );
}

void Player::Impl::printMemBoard( const Pcell& printPcell )
{
    cout << "---------------"<< endl;
    for( int i = (ROWS - 1); i >= 0; --i )
    {
        for( int j = 0; j < COLS; ++j )
           cout <<  printPcell->b->getPawn(i, j).getType();

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

/// End of Impl implementation ////////////////////////////////////////////////

/// Start of Pawn class implementation ////////////////////////////////////////

Pawn::Pawn()
{
    x     = 0;
    y     = 0;
    type  = 'e';
    queen = false;
}

Pawn::Pawn( char t, int row, int col, int playerNr )
{
    type   = t;
    x      = col;
    y      = row;
    player = playerNr;

    if(type == 'X' || type == 'O')
        queen = true;
}

Pawn::Pawn( Pawn& p )
{
    x      = p.x;
    y      = p.y;
    type   = p.type;
    player = p.player;
    queen  = p.queen;
}

void Pawn::setValues(char t, int row, int col)
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

void Pawn::printType()
{
    cout << type << endl;
}

void Pawn::deletePawn()
{
    type   = 'e';
    player =  0;
    queen  = false;
}

void Pawn::setQueen()
{
    queen = true;
    type == 'x' ? type = 'X': type = 'O';
}

void Pawn::setCoordinates(int row, int col)
{
    if(checkValidCoordinates(row, col))
    {
        x = col;
        y = row;
    }
}

bool Pawn::checkValidCoordinates(int row, int col)
{
    if(row < 0 || row > 7 || col < 0 || col > 14)
        exit (1);
    return true;
}

bool Pawn::isQueen()
{
    if(queen)
        return true;

    return false;
}

int Pawn::getX()
{
    return x;
}

int Pawn::getY()
{
    return y;
}

char Pawn::getType()
{
    return type;
}

int Pawn::getPlayer()
{
    return player;
}

Pawn Pawn::operator = (Pawn& p)
{
    type   = p.type;
    queen  = p.queen;
    player = p.player;

    return *this;
}

/// End of Pawn class implementation //////////////////////////////////////////

/// Start Board class implementation //////////////////////////////////////////

Board::Board( char b[ROWS][COLS] )
{
    for( int i = ( ROWS - 1 ); i >= 0; --i )
        for( int j = 0; j < COLS; ++j )
            board[ i ][ j ].setValues( b[ i ][ j ], i, j );
    updatePieceLeft();
}

Board::Board( Board& source )
{
    for( int i = ( ROWS - 1 ); i >= 0; --i )
        for( int j = 0; j < COLS; ++j )
            board[ i ][ j ].setValues( source.board[ i ][ j ].getType(), i, j );
    updatePieceLeft();
}

Pawn& Board::getPawn( int row, int col )
{
    return board[ row ][ col ];
}

void Board::printBoard()
{
    cout << "---------------"<< endl;
    for( int i = ( ROWS - 1 ); i >= 0; --i )
    {
         for( int j = 0; j < COLS; ++j )
            board[ i ][ j ].getType() == 'e' ? cout << ' ' :  cout << board[ i ][ j ].getType();

         cout << endl;
    }
    cout << "---------------\n"<< endl;
}

void Board::move(Pawn& piece,int row, int col)
{
    Pawn support(board[row][col]);

    board[row][col] = piece;
    piece = support;

    if( row == (ROWS - 1) || row == 0 )
        board[row][col].setQueen();
}

void Board::updatePieceLeft()
{
    xLeft = xKing = 0;
    oLeft = oKing = 0;
    for( int i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( int j = 0; j < COLS; ++j )
        {
            switch ( board[ i ][ j ].getType())
            {
                case 'x':
                     ++xLeft;
                     break;
                case 'X':
                    ++xKing;
                    break;

                case 'o':
                    ++oLeft;
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

void Board::append( Pcell& newCell, int player )
{
    if( player == 1 )
    {
        if( xHead == nullptr )
        {
            xHead = newCell;
            xTail = newCell;
            newCell->prev = nullptr;
        }
        else
        {
            xTail->next = newCell;
            newCell->prev = xTail;
            xTail = newCell;
        }

        xTail->next = nullptr;
    }
    else
    {
        if( oHead == nullptr )
        {
            oHead = newCell;
            oTail = newCell;
            newCell->prev = nullptr;
        }
        else
        {
            oTail->next = newCell;
            newCell->prev = oTail;
            oTail = newCell;
        }

        oTail->next = nullptr;
    }
}

void Board::initHead()
{
    xHead = nullptr;
    oHead = nullptr;
    xTail = nullptr;
    oTail = nullptr;
}

int Board::getXLeft()
{
    return xLeft;
}


int Board::getOLeft()
{
    return oLeft;
}

int Board::wins()
{
    if( ( oLeft + oKing ) == 0 )
        return 1;
    else if( ( xLeft + xKing ) == 0 )
        return 2;

    return 0;
}
void Board::destroy( Pcell node ) const
{
    if( node )
    {
        destroy( node->next );
        delete node;
    }
}

Pcell Board::getValidMoves( Pawn& p )
{
    int   left  = p.getX() - 2;
    int   right = p.getX() + 2;
    int   row   = p.getY();

    if( p.getType() == 'x' || p.isQueen() )
    {
        traverseLeft ( row + 1, std::max( row + 3, -1 ), 1, p, left,  std::pair<int, int>( 0, 0 ) );
        traverseRight( row + 1, std::max( row + 3, -1 ), 1, p, right, std::pair<int, int>( 0, 0 ) );
    }
    if( p.getType() == 'o' || p.isQueen() )
    {
        traverseLeft ( row - 1, std::max( row - 3, -1 ), -1, p, left,  std::pair<int, int>( 0, 0 ) );
        traverseRight( row - 1, std::max( row - 3, -1 ), -1, p, right, std::pair<int, int>( 0, 0 ) );
    }

    if( p.getPlayer() == 1 )
        return xHead;
    return oHead;
}

void Board::traverseLeft( int start, int stop, int step, Pawn pawn, int left, std::pair< int, int > lastPos )
{
    bool eatable = false;
    bool moved   = false;
    bool blocked = false;

    for( int row = start; row < ROWS && row != stop && !moved; row += step )
    {
        if( left < 0 )
            break;

        Pawn current = getPawn( row, left );
        if( current.getType() == 'e' && !blocked )
        {
            Pcell newNode = new Cell;
            newNode->b = new Board( *this );
            newNode->b->move( newNode->b->getPawn( pawn.getY(), pawn.getX() ), current.getY(), current.getX() );

            if( eatable )
            {
                newNode->b->getPawn( lastPos.first, lastPos.second ).deletePawn();
                newNode->b->updatePieceLeft();
            }
            append( newNode, pawn.getPlayer() );
            moved = true ;
        }
        else if( ( current.getPlayer() == pawn.getPlayer() ) || ( current.isQueen() && !pawn.isQueen() ) )
            blocked = true;
        else
        {
            eatable = true;
            lastPos.first = current.getY();
            lastPos.second = current.getX();
        }

        left -= 2;
    }
}

void Board::traverseRight( int start, int stop, int step, Pawn pawn, int right,  std::pair< int, int > lastPos )
{
    bool eatable = false;
    bool moved   = false;
    bool blocked = false;


    for( int row = start; row < ROWS && row != stop && !moved; row += step )
    {
            if( right >= COLS )
                break;

            Pawn current = getPawn( row, right );
            if( current.getType() == 'e' && !blocked )
            {
                Pcell newNode = new Cell;
                newNode->b = new Board( *this );
                newNode->b->move( newNode->b->getPawn( pawn.getY(), pawn.getX()), current.getY(), current.getX() );

                if( eatable )
                {
                    newNode->b->getPawn( lastPos.first, lastPos.second ).deletePawn();
                    newNode->b->updatePieceLeft();
                }
                append( newNode, pawn.getPlayer() );
                moved = true;
            }
            else if( current.getPlayer() == pawn.getPlayer() || ( current.isQueen() && !pawn.isQueen() ) )
                blocked = true;
            else
            {
                eatable = true;
                lastPos.first = current.getY();
                lastPos.second = current.getX();
            }

            right += 2;
    }
}

double Board::evalutation()
{
    return ( ( xLeft - oLeft ) + ( oKing * 1.5 - xKing * 1.5 ) );
}

Pair Player::Impl::minimax( Pboard& board, int depth, bool maxPlayer )
{
    if( ( depth == 0 ) || ( board->wins() != 0 ) )
    {
        Pair bestMove;
        bestMove.score = board->evalutation();
        bestMove.pio   = board;
        cout << "Forse valore brutto: " << bestMove.score << endl;
        return bestMove;
    }

    if( maxPlayer )
    {
        Pboard bestBoard = board;
        Pair evaluation;
        double maxEval = MINF;
        for( int i = ( ROWS - 1 ); i >= 0; --i )
        {
            for( int j = 0; j < COLS; ++j )
            {
                if( board->getPawn( i, j ).getPlayer() == 1 )
                {
                    Pcell currentPiece = board->getValidMoves( board->getPawn( i, j ) );
                    if( currentPiece )
                    {
                        while( currentPiece != nullptr )
                        {
                            evaluation = minimax( currentPiece->b, ( depth - 1 ), false );
                            maxEval    = std::max( maxEval, evaluation.score );

                            if( maxEval == evaluation.score )
                                bestBoard = currentPiece->b;
                            currentPiece = currentPiece->next;
                        }
                    }
                }
            }
        }
        Pair bestMove;
        bestMove.pio   = bestBoard;
        bestMove.score = maxEval;
        return bestMove;
    }

    Pboard bestBoard = board;
    Pair evaluation;
    double minEval = PINF;
    for( int i = ( ROWS - 1 ); i >= 0; --i )
    {
        for( int j = 0; j < COLS; ++j )
        {
            if( board->getPawn( i, j ).getPlayer() == 2 )
            {
                Pcell currentPiece = board->getValidMoves( board->getPawn( i, j ) );

                if( currentPiece != nullptr )
                {
                    while( currentPiece != nullptr )
                    {
                        evaluation = minimax(  currentPiece->b, ( depth - 1 ), true );
                        minEval         = std::min( minEval, evaluation.score );

                        if( minEval == evaluation.score )
                            bestBoard = currentPiece->b;
                        currentPiece = currentPiece->next;
                    }
                }
            }
        }
    }

    Pair bestMove;
    bestMove.pio   = bestBoard;
    bestMove.score = minEval;
    return bestMove;
}

/// End Board class implementation ////////////////////////////////////////////
