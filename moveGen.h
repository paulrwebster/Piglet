#pragma once
using namespace std;
#include <string>
#include <vector>
#include <iostream>
#include "gameboard.h"
#include "defs.h"

class MoveGen
{
public:
	MoveGen();
	~MoveGen();
	typedef struct mvlist {
		int move;
		int score;
		bool quiet;
		bool operator==(const int rhs) const
		{
			return move == rhs;
		}  //for error C2676 on std::find (passing a move as an int)
		bool operator>(const mvlist mv2)
		{
			return score > mv2.score;
		} //for sorting structure
		bool operator<(const mvlist mv2)
		{
			return score < mv2.score;
		} //for sorting structure
	} moveList;

	
	
	
	vector <moveList> generateMoves(Gameboard& Board, int ply);
	vector <moveList> generateCaptureMoves(Gameboard& Board, int ply);
	bool blackPawnMove(Gameboard& Board, int ply,int square);
	bool blackPawnCaptureMove(Gameboard& Board, int ply, int square, bool capturesOnly);
	bool blackPawnPromotionForQuiescence(Gameboard& Board, int ply, int square);
	bool whitePawnMove(Gameboard& Board, int ply, int square);
	bool whitePawnCaptureMove(Gameboard& Board, int ply,  int square, bool capturesOnly);
	bool whitePawnPromotionForQuiescence(Gameboard& Board, int ply, int square);
	bool kingMove(Gameboard& Board, int ply, int square, char side, bool capturesOnly);
	bool whiteKingSideCastle(Gameboard& Board, int ply);
	bool whiteQueenSideCastle(Gameboard& Board, int ply);
	bool blackKingSideCastle(Gameboard& Board, int ply);
	bool blackQueenSideCastle(Gameboard& Board, int ply);
	bool knightMove(Gameboard& Board, int ply, int square, char side, bool capturesOnly);
	bool rookMove(Gameboard& Board, int ply, int square, char side, bool capturesOnly);
	bool bishopMove(Gameboard& Board, int ply, int square, char side, bool capturesOnly);
	void addMove(int from, int to, Gameboard& Board, int ply, int enpassant, int promoted, bool capturesOnly);
	int getCaptureScore(int from, int to, Gameboard& Board, int ply);
	int getKillerScore(int move, int ply);
	int getHistoryScore(int from, int to, Gameboard& Board);
	void printMoves(Gameboard& Board);
	void printMove(int move, Gameboard& Board);
	int notationToMoveRepresentation(string move, Gameboard& Board);
	string moveRepresentationToNotation(int move);
	int fromSquare(int move);
	int toSquare(int move);
	int getCaptured(int move);
	void initialiseMoves();
	vector <moveList> getMoves();
	int setMoveCastleFlag(Gameboard& Board, int move);
	int getMoveCastleFlag(int move);
	void setMoveEnPassantFlag(int& move);
	//void setMoveEnPassantFlag(int& move, int square);
	void setMoveEnPassantColumn(int& move, int column);
	int getMoveEnPassantFlag(int move);
	int getMoveEnPassantColumn(int move);
	void clearMoveEnPassantFlag(int& move);
	void setPromoted(int move, int piece);
	int getPromoted(int move);
	void printMoveRepresentation(int move);
	//attacks
	void setAttacksArray();
	void testAttacks();
	//bool inCheck(int kingSquare, Gameboard& Board, char side); //rewritten as isInCheck
	bool knightCheck(int kingSquare, Gameboard& Board, int delta, char side); //Todo should be private but using in dev()
	//bool squareAttacked(int square, Gameboard& Board, char side);
	bool squareAttacked(int square, Gameboard& Board, char side);
	int getNumberOfNodes();
	//bool isInCheck(Gameboard& Board);
	bool isInCheck(Gameboard& Board, int colour);
	bool isInCheck(int kingSquare, Gameboard& Board, char side);
	void initMvvLva();
	int getMvvLvaScores(int arrayIndex);
private:
	//vector <int> moves;
	vector <moveList> moves;
	unsigned int Attacks[257]{ 0 }; //-128 to 128
	bool slidingPieceCheck(int kingSquare, Gameboard& Board, int delta, char side);
	bool pawnPieceCheck(int kingSquare, Gameboard& Board, int delta, char side);
	int numberOfNodes = 0;
	int mvvLvaValue[13] = { 0,100,200,300,400,500,600,100,200,300,400,500,600 };
	int mvvLvaScores[14 * 14];
};






