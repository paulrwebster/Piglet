#include "hashing.h"
#include "gameboard.h"
#include "moveGen.h"

// define TT instance
Hashing::transpositionTable hash_table[HASHSIZE];

//Public constructor
Hashing::Hashing()
{

}

Hashing::~Hashing()
{

}

void Hashing::generateHashKeys() 
{
    twister.seed(5489u); //standard seed
    

    for (int i = 0; i < 781; i++)
    {
        hashkeys[i] = twister();
    }
}

void Hashing::clearHashTable()
{
    // loop over TT elements
    for (int index = 0; index < HASHSIZE; index++)
    {
        // reset TT inner fields
        hash_table[index].key = 0;
        hash_table[index].depth = 0;
        hash_table[index].flags = 0;
        hash_table[index].value = 0;
    }
}

void Hashing::setZobristEp(int square)
{
    int file = square & 7;
    zobristKey ^= hashkeys[773 + file];
}

void Hashing::setZobristPiece(int square, Defs::Pieces piece)
{
    if (piece > Defs::Pieces::empty && piece <= Defs::Pieces::enpassant )
    {
        int file = (square & 7);
        int rank = (square >> 4);
        int sq64 = (rank * 8) + (file + 1);

        zobristKey ^= hashkeys[(sq64 * static_cast<int>(piece)) - 1];

    }
}

void Hashing::setZobristSide()
{
     zobristKey ^= 768;  //set if black. Reverse if white
}


void Hashing::setZobristCastle(int castleflag)
{
    if (castleflag & 1) { zobristKey ^= 769; }
    if (castleflag & 2) { zobristKey ^= 770; }
    if (castleflag & 4) { zobristKey ^= 771; }
    if (castleflag & 8) { zobristKey ^= 772; }
}



unsigned long long Hashing::generatePositionKey(Gameboard& Board)
{
Defs::Pieces piece;
zobristKey = 0;

//unsigned long long hashkeys[781]
//(12*64 pieces on squares  - 0 to 767
//+ 1 side black            - 768
//+ 4 castling rights       - 769 to 772
//+ 8 enpassant file)       - 773 to 780

for (int i = 0; i < 128; ++i) //iterate through the sqaures
    {
        piece = Board.getPiece(i);
        if (piece == Defs::Pieces::offboard) { continue; }
        else if (piece == Defs::Pieces::empty) { continue; }
        else if (piece == Defs::Pieces::enpassant)
        {
            setZobristEp(i);
        }
            
        else 
        {
            setZobristPiece(i, piece);
        }
        
    }

if (Board.getSide() == 'b')
{   
    setZobristSide();
}

setZobristCastle(Board.getCastleFlag());
return zobristKey;
}


// read hash entry data
int Hashing::ProbeHash(int depth, int alpha, int beta)
{
    // create a TT instance pointer to particular hash entry storing
    // the scoring data for the current board position if available
    transpositionTable* transpoTable = &hash_table[zobristKey % HASHSIZE];
    // make sure we're dealing with the exact position we need
    if (transpoTable->key == zobristKey) {
        // make sure that we match the exact depth our search is now at
        if (transpoTable->depth >= depth) {
            // match the exact (PV node) score 
            if (transpoTable->flags == hashfEXACT)
            {   
                // return exact (PV node) score
                return transpoTable->value;
            }
            // match alpha (fail-low node) score
            if ((transpoTable->flags == hashfALPHA) &&  //value of the node was at most value
                (transpoTable->value <= alpha))
            {   
                // return alpha (fail-low node) score
                return alpha;
            }
            // match beta (fail-high node) score
            if ((transpoTable->flags == hashfBETA) &&  //value of the node was at least value
                (transpoTable->value >= beta))
            {
                // return beta (fail-high node) score
                return beta;
            }
        }
    }
    //if hash entry doesn't exist
    return VALUNKNOWN; //Value Unknown 1000000 greater than alpha beta infinity cutoff of 999999
}



// write hash entry data
void Hashing::RecordHash(int depth, int val, int hashf)

{
    // create a TT instance pointer to particular hash entry storing
    // the scoring data for the current board position if available
    transpositionTable* transpoTable = &hash_table[zobristKey % HASHSIZE];
   
    // write hash entry data 
    transpoTable->key = zobristKey;
    transpoTable->value = val;
    transpoTable->flags = hashf;
    transpoTable->depth = depth;
}

unsigned long long Hashing::getZobristKey()
{
    return zobristKey;
}

