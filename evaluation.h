#pragma once
#include "gameboard.h"

class Evaluation
{
public:
int evaluateBoard(Gameboard &Board, char player);
int evaluateBoard(Gameboard& Board); //calls evaluateWhite
int evaluate(Defs::Pieces piece, int square);
int evaluateWhitePerspective(Defs::Pieces piece, int square);
int getPieceVal(int pieceIndex);


private:
	 //      empty, P,  N,  B,  R,  Q,  K,    p,  n,  b,  r,  q,   k, offboard, ep
int pieceVal[15]{0,100,320,330,500,900,20000,100,320,330,500,900,20000,0,0}; //k was 20000

//Piece square tables -- Tomasz Michniewski 

//N.B. We are displaying the board upside down, so top row of evaluation array is bottom row of board  

//Pawns - we simply encourage the pawns to advance. Additionally we try to discourage the engine from leaving central pawns unmoved.
//Bonus for advanced pawns
int blackPawnEval[128]{
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
50, 50, 50, 50, 50, 50, 50, 50, 0,  0,  0,  0,  0,  0,  0,  0,
10, 10, 20, 30, 30, 20, 10, 10, 0,  0,  0,  0,  0,  0,  0,  0,
 5,  5, 10, 25, 25, 10,  5,  5, 0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0, 20, 20,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,
 5, -5,-10,  0,  0,-10, -5,  5, 0,  0,  0,  0,  0,  0,  0,  0,
 5, 10, 10,-20,-20, 10, 10,  5, 0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0
};

int whitePawnEval[128]{
  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10,-20,-20, 10, 10,  5, 0,  0,  0,  0,  0,  0,  0,  0,
  5, -5,-10,  0,  0,-10, -5,  5, 0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 20, 20,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,
  5,  5, 10, 25, 25, 10,  5,  5, 0,  0,  0,  0,  0,  0,  0,  0,
 10, 10, 20, 30, 30, 20, 10, 10, 0,  0,  0,  0,  0,  0,  0,  0,
 50, 50, 50, 50, 50, 50, 50, 50, 0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0 
};

//Knights - Encourage them to go to the centre
int blackKnightEval[128]{
-50,-40,-30,-30,-30,-30,-40,-50, 0,  0,  0,  0,  0,  0,  0,  0,
-40,-20,  0,  0,  0,  0,-20,-40, 0,  0,  0,  0,  0,  0,  0,  0,
-30,  0, 10, 15, 15, 10,  0,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,  5, 15, 20, 20, 15,  5,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,  0, 15, 20, 20, 15,  0,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,  5, 10, 15, 15, 10,  5,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-40,-20,  0,  5,  5,  0,-20,-40, 0,  0,  0,  0,  0,  0,  0,  0,
-50,-40,-30,-30,-30,-30,-40,-50, 0,  0,  0,  0,  0,  0,  0,  0
};

int whiteKnightEval[128]{
-50,-40,-30,-30,-30,-30,-40,-50, 0,  0,  0,  0,  0,  0,  0,  0,
-40,-20,  0,  5,  5,  0,-20,-40, 0,  0,  0,  0,  0,  0,  0,  0,
-30,  5, 10, 15, 15, 10,  5,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,  0, 15, 20, 20, 15,  0,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,  5, 15, 20, 20, 15,  5,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,  0, 10, 15, 15, 10,  0,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-40,-20,  0,  0,  0,  0,-20,-40, 0,  0,  0,  0,  0,  0,  0,  0,
-50,-40,-30,-30,-30,-30,-40,-50, 0,  0,  0,  0,  0,  0,  0,  0
};
//Bishops - We avoid corners and borders. Additionally we prefer squares like b3, c4, b5, d3 and the central ones. 
int blackBishopEval[128]{
-20,-10,-10,-10,-10,-10,-10,-20, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0,  0,  0,  0,  0,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0,  5, 10, 10,  5,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  5,  5, 10, 10,  5,  5,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0, 10, 10, 10, 10,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10, 10, 10, 10, 10, 10, 10,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  5,  0,  0,  0,  0,  5,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-20,-10,-10,-10,-10,-10,-10,-20, 0,  0,  0,  0,  0,  0,  0,  0    
};

int whiteBishopEval[128]{
-20,-10,-10,-10,-10,-10,-10,-20, 0,  0,  0,  0,  0,  0,  0,  0,    
-10,  5,  0,  0,  0,  0,  5,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10, 10, 10, 10, 10, 10, 10,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0, 10, 10, 10, 10,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  5,  5, 10, 10,  5,  5,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0,  5, 10, 10,  5,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0,  0,  0,  0,  0,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-20,-10,-10,-10,-10,-10,-10,-20, 0,  0,  0,  0,  0,  0,  0,  0
};

//Rooks - centralise, occupy the 7th rank and avoid a, h columns
 int blackRookEval[128]{
  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  0,  0,  0,  0,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  0,  0,  0,  0,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  0,  0,  0,  0,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  0,  0,  0,  0,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  0,  0,  0,  0,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  5,  5,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0   
 };

int whiteRookEval[128]{
  0,  0,  0,  5,  5,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,   
 -5,  0,  0,  0,  0,  0,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  0,  0,  0,  0,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  0,  0,  0,  0,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  0,  0,  0,  0,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  0,  0,  0,  0,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
 5, 10, 10, 10, 10, 10, 10,  5, 0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0
 };

 int blackQueenEval[128]{
-20,-10,-10, -5, -5,-10,-10,-20, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0,  0,  0,  0,  0,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0,  5,  5,  5,  5,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  5,  5,  5,  5,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  5,  5,  5,  5,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  5,  5,  5,  5,  5,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0,  5,  0,  0,  0,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-20,-10,-10, -5, -5,-10,-10,-20, 0,  0,  0,  0,  0,  0,  0,  0   
 };

int whiteQueenEval[128]{
-20,-10,-10, -5, -5,-10,-10,-20, 0,  0,  0,  0,  0,  0,  0,  0,   
-10,  0,  5,  0,  0,  0,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  5,  5,  5,  5,  5,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  5,  5,  5,  5,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
 -5,  0,  5,  5,  5,  5,  0, -5, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0,  5,  5,  5,  5,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-10,  0,  0,  0,  0,  0,  0,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-20,-10,-10, -5, -5,-10,-10,-20, 0,  0,  0,  0,  0,  0,  0,  0
 };


//King = make the king stand behind the pawn shelter.
 int blackKingMiddleGameEval[128]{
-30,-40,-40,-50,-50,-40,-40,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-40,-40,-50,-50,-40,-40,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-40,-40,-50,-50,-40,-40,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-40,-40,-50,-50,-40,-40,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-20,-30,-30,-40,-40,-30,-30,-20, 0,  0,  0,  0,  0,  0,  0,  0,
-10,-20,-20,-20,-20,-20,-20,-10, 0,  0,  0,  0,  0,  0,  0,  0,
 20, 20,  0,  0,  0,  0, 20, 20, 0,  0,  0,  0,  0,  0,  0,  0,
 20, 30, 10,  0,  0, 10, 30, 20, 0,  0,  0,  0,  0,  0,  0,  0   
 };

 int whiteKingMiddleGameEval[128]{
 20, 30, 10,  0,  0, 10, 30, 20, 0,  0,  0,  0,  0,  0,  0,  0,
 20, 20,  0,  0,  0,  0, 20, 20, 0,  0,  0,  0,  0,  0,  0,  0,
-10,-20,-20,-20,-20,-20,-20,-10, 0,  0,  0,  0,  0,  0,  0,  0,
-20,-30,-30,-40,-40,-30,-30,-20, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-40,-40,-50,-50,-40,-40,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-40,-40,-50,-50,-40,-40,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-40,-40,-50,-50,-40,-40,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-40,-40,-50,-50,-40,-40,-30, 0,  0,  0,  0,  0,  0,  0,  0
 };


//end game is both sides no queens or a queen and one minor piece 
 int blackKingEndGameEval[128]{
-50,-40,-30,-20,-20,-30,-40,-50, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-20,-10,  0,  0,-10,-20,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-10, 20, 30, 30, 20,-10,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-10, 30, 40, 40, 30,-10,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-10, 30, 40, 40, 30,-10,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-10, 20, 30, 30, 20,-10,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-30,  0,  0,  0,  0,-30,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-50,-30,-30,-30,-30,-30,-30,-50, 0,  0,  0,  0,  0,  0,  0,  0
 };

int whiteKingEndGameEval[128]{
-50,-30,-30,-30,-30,-30,-30,-50, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-30,  0,  0,  0,  0,-30,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-10, 20, 30, 30, 20,-10,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-10, 30, 40, 40, 30,-10,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-10, 30, 40, 40, 30,-10,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-10, 20, 30, 30, 20,-10,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-30,-20,-10,  0,  0,-10,-20,-30, 0,  0,  0,  0,  0,  0,  0,  0,
-50,-40,-30,-20,-20,-30,-40,-50, 0,  0,  0,  0,  0,  0,  0,  0
 };

};

