#ifndef _HASHING_H_
#define _HASHING_H_

#include <random>
#include <iostream>
#include "gameboard.h"
#include "moveGen.h"

class Hashing {
public:
	Hashing();
	~Hashing();
	//static Hashing* instance;
	//static Hashing* getInstance();
	void generateHashKeys();
	void clearHashTable();
	unsigned long long generatePositionKey(Gameboard& Board);
	int ProbeHash(int depth, int alpha, int beta);
	void RecordHash(int depth, int val, int hashf);
	typedef struct {
		unsigned long long key; //zobrist key
		int depth;				//depth of search
		int flags;				//hashfEXACT 0, hashfALPHA 1, hashfBETA 2
		int value;				//If hashfEXACT,, the evaluation. If flag hashfALPHA, then at most the evaluation. If hashfBETA, at leat the evaluation
		int bestMove;
	}  transpositionTable;
	void setZobristEp(int square);
	void setZobristPiece(int square, Defs::Pieces piece);
	void setZobristSide();
	void setZobristCastle(int castleflag);
	unsigned long long getZobristKey();
	
//private:
	//Hashing();//Private constructor
	//~Hashing();
	std::mt19937_64 twister; //mersenne twister
	unsigned long long hashkeys[781] = {}; //(12*64 pieces on squares + 1 side black + 4 castling rights + 8 enpassant file)
	unsigned long long zobristKey = 0;

};

#endif