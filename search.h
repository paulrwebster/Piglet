#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "gameboard.h"
#include "moveGen.h"
#include "evaluation.h"
#include <iostream>
#include <vector>
#include <utility>
#include <thread>
#include <algorithm>
#include "hashing.h"
#include "defs.h"


class Search {
public:
	typedef struct tagLINE {
		int cmove = 0;       // Number of moves in the line.
		int argmove[Defs::MaxSearchDepth]{};  // The line.
		unsigned long long hash[Defs::MaxSearchDepth]{}; //holds the board position hash 
	}   LINE;

	int negamax(std::stop_token st, Gameboard& Board, Hashing& Hash, int depth, int pvDepth, int ponderDepth, int alpha, int beta, LINE* pline, int timePerMove, int mate);
	int quiescence(Gameboard& Board, Hashing& QHash, int alpha, int beta);
	int quiescence(Gameboard& Board, Hashing& QHash, int depth, int alpha, int beta, int timePerMove); //depth limited quiescence
	int getBestMoveFound();
	int getPreviousBestMove();
	void setPreviousBestMove(int bestMove);
	void setPreviousBestLine(LINE bestLine);
	void setPreviousRetVal(int retVal);
	LINE getBestLine();
	LINE getPreviousBestLine();
	int getPreviousRetval();
	size_t getNumberOfNodes();
	size_t getNumberOfQNodes();
	void resetNumberOfNodes();
	size_t getNumberOfHashMatches();
	size_t getNumberOfQHashMatches();
	void resetNumberOfHashMatches();
	void resetNumberOfQHashMatches();
	int reorderMoves(int bestMove, vector <MoveGen::moveList> *moves); //reorder to best move first for iterative deepening
	long long perft(Gameboard &Board, Hashing& Hash, int depth);
	long long perftCheckHash(Gameboard& Board, Hashing& Hash, Hashing& CheckHash, int depth, string parentMove);
	long long divide(Gameboard &Board, Hashing& Hash, int depth); //perft divide
	long long divideCheckHash(Gameboard& Board, Hashing& Hash, Hashing& CheckHash, int depth); //perft divide plus verifies Hash values
	void resetFailHigh();
	int getFailHigh();
	int getFailHighFirst();
	int getFailQHigh();
	int getFailQHighFirst();
	void initMoveGenMvvLva();
	int probePVTable(LINE line, unsigned long long boardHash);
	void clearSearch();
	void clearBestMove();
	void clearPreviousBestMove();
	void clearBestLine();
	void clearPreviousBestLine();
	void stopEngine();
	void startEngine();
	void startPondering();
	void stopPondering();
	bool isPondering();
	bool isStopped();
	bool isPonderhit();
	void setPonderhit(bool hit);
	void setInfinite(bool isInfinite);
	bool isInfinite();

private:
	bool stop = false;
	bool pondering = false;
	bool ponderhit = false;
	bool infinite = false;
	size_t numberOfNodes = 0;
	size_t numberOfQNodes = 0; //quiescence nodes
	size_t numberOfHashMatches = 0;
	size_t numberOfQHashMatches = 0; //quiescence hash matches
	int bestMove = 0;
	int previousBestMove = 0;
	LINE bestLine;
	LINE previousBestLine;
	int previousRetVal = 0;
	int quiescenceDepth = 6;
	int perftLeafNodes = 0;
	void printNodes(LINE* pline, int val, Gameboard& Board, int depth);
	int failHigh = 0;
	int failHighFirst = 0;
	int failQHigh = 0; //quiescence ordering stats
	int failQHighFirst = 0;
	long long nodes = 0;
};

#endif