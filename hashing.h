#ifndef _HASHING_H_
#define _HASHING_H_

#include <random>
#include <iostream>
#include "gameboard.h"
#include "moveGen.h"
#include<set>

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
	bool probeHistory(unsigned long long zobrist);
	void insertHistory(unsigned long long zobrist);
	void clearHistory();
	void printHistory();
	
	
	std::mt19937_64 twister; //mersenne twister
	unsigned long long hashkeys[781] = {}; //(12*64 pieces on squares + 1 side black + 4 castling rights + 8 enpassant file)
	unsigned long long zobristKey = 0;
private:
	std::set<unsigned long long> historySet = {}; //populated with zobrist keys of board positions that have occurred at least twice, used for 3 fold repetition
};

#endif