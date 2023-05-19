#include "moveGen.h"
#include "evaluation.h"
#include <bitset>

extern int killers[2][Defs::MaxSearchDepth];
extern int searchHistory[15][128]; //[number of pieces][number of squares]

MoveGen::MoveGen()
{
	setAttacksArray();
}

MoveGen::~MoveGen()
{

}



vector <MoveGen::moveList> MoveGen::generateMoves(Gameboard &Board, int ply) 
{
	initialiseMoves();
	char side = Board.getSide();
		
	int piece = 0, pieceCnt = 0;
	//Adding big pieces to move list, first, appears to considerably improve pruning	
	//White moves
	if (side == 'w')
	{
		Board.clearEnpassantFlag('w');
		piece = static_cast<int>(Defs::Pieces::K);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			kingMove(Board, ply, i, side, false);
			whiteKingSideCastle(Board, ply);
			whiteQueenSideCastle(Board, ply);
		}
		piece = static_cast<int>(Defs::Pieces::Q);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			rookMove(Board, ply, i, side, false);
			bishopMove(Board, ply, i, side, false);
		}
		piece = static_cast<int>(Defs::Pieces::R);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			rookMove(Board, ply, i, side, false);
		}

		piece = static_cast<int>(Defs::Pieces::B);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			bishopMove(Board, ply, i, side, false);
		}

		piece = static_cast<int>(Defs::Pieces::N);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			knightMove(Board, ply, i, side, false);
		}

		piece = static_cast<int>(Defs::Pieces::P);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			whitePawnMove(Board, ply, i);
			whitePawnCaptureMove(Board, ply, i, false);
		}
		
	}

	//black moves
	if (side == 'b')
	{
		Board.clearEnpassantFlag('b');
		
		piece = static_cast<int>(Defs::Pieces::k);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			kingMove(Board, ply, i, side, false);
			blackKingSideCastle(Board, ply);
			blackQueenSideCastle(Board, ply);
		}

		piece = static_cast<int>(Defs::Pieces::q);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			rookMove(Board, ply, i, side, false);
			bishopMove(Board, ply, i, side, false);
		}

		piece = static_cast<int>(Defs::Pieces::r);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			rookMove(Board, ply, i, side, false);
		}

		piece = static_cast<int>(Defs::Pieces::b);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			bishopMove(Board, ply, i, side, false);
		}

		piece = static_cast<int>(Defs::Pieces::n);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			knightMove(Board, ply, i, side, false);
		}

		piece = static_cast<int>(Defs::Pieces::p);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			blackPawnMove(Board, ply, i);
			blackPawnCaptureMove(Board, ply, i, false);

		}

	}
	
	return moves;
}

vector <MoveGen::moveList> MoveGen::generateCaptureMoves(Gameboard& Board, int ply)
{
	//want to record captues, pawn promotions and checks for the quiescence function

	initialiseMoves();
	char side = Board.getSide();

	int piece = 0, pieceCnt = 0;
	//Adding big pieces to move list, first, appears to considerably improve pruning	
	//White moves
	if (side == 'w')
	{
		Board.clearEnpassantFlag('w');
		piece = static_cast<int>(Defs::Pieces::K);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			kingMove(Board, ply, i, side, true);
		}
		piece = static_cast<int>(Defs::Pieces::Q);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			rookMove(Board, ply, i, side, true);
			bishopMove(Board, ply, i, side, true);
		}
		piece = static_cast<int>(Defs::Pieces::R);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			rookMove(Board, ply, i, side, true);
		}

		piece = static_cast<int>(Defs::Pieces::B);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			bishopMove(Board, ply, i, side, true);
		}

		piece = static_cast<int>(Defs::Pieces::N);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			knightMove(Board, ply, i, side, true);
		}

		piece = static_cast<int>(Defs::Pieces::P);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			whitePawnCaptureMove(Board, ply, i, true);
			//whitePawnPromotionForQuiescence(Board, ply, i);
		}

	}

	//black moves
	if (side == 'b')
	{
		Board.clearEnpassantFlag('b');

		piece = static_cast<int>(Defs::Pieces::k);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			kingMove(Board, ply, i, side, true);
		}

		piece = static_cast<int>(Defs::Pieces::q);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			rookMove(Board, ply, i, side, true);
			bishopMove(Board, ply, i, side, true);
		}

		piece = static_cast<int>(Defs::Pieces::r);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			rookMove(Board, ply, i, side, true);
		}

		piece = static_cast<int>(Defs::Pieces::b);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			bishopMove(Board, ply, i, side, true);
		}

		piece = static_cast<int>(Defs::Pieces::n);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			knightMove(Board, ply, i, side, true);
		}

		piece = static_cast<int>(Defs::Pieces::p);
		for (pieceCnt = 1; pieceCnt <= Board.pieceCount[piece]; ++pieceCnt)
		{
			int i = Board.getPieceSquare(piece, pieceCnt);
			blackPawnCaptureMove(Board, ply, i, true);
		}

	}

	return moves;
}


int MoveGen::getNumberOfNodes()
{
	return numberOfNodes;
}

bool MoveGen::knightMove(Gameboard& Board, int ply, int square, char side, bool capturesOnly)
{
	int knMoves[8]{ 31,33,18,14,-31,-33,-18,-14 }; 
	for (int i = 0; i < 8; i++)
	{
		int squareTo = square + knMoves[i];
		if (Board.onBoard(squareTo) == false) { continue; }  //off board
		if (Board.getPieceColour((int)Board.getPiece(squareTo)) == side) { continue; } //occupied by own side
			addMove(square, squareTo, Board, ply, 0, 0, capturesOnly);
	}
	return true;
}

bool MoveGen::kingMove(Gameboard& Board, int ply, int square, char side, bool capturesOnly)
{
	
	int kMoves[8]{ 1,-1,16,-16,15,17,-15,-17 };
	for (int i = 0; i < 8; i++)
	{
		int squareTo = square + kMoves[i];
		if (Board.onBoard(squareTo) == false) { continue; }  //off board
		if (Board.getPieceColour((int)Board.getPiece(squareTo)) == side) { continue; } //occupied by own side
		//cout << "King Move" << squareTo << endl;
		//Board.printBoard();
		//if (isInCheck(squareTo, Board, side) == true) { continue; } //moving into check
		
		addMove(square, squareTo, Board, ply, 0, 0, capturesOnly);
	}
	return true;
}

bool MoveGen::whiteKingSideCastle(Gameboard& Board, int ply )
{
	int square = 4; //E1 white castle starting position
	if (Board.getCastleFlag() & 8)
	{
		if (isInCheck(square, Board, 'w') == false){
			if (isInCheck(square + 1, Board, 'w') == false) {
				if (isInCheck(square + 2, Board, 'w') == false) {
					if (Board.getPiece(square + 1) == Defs::Pieces::empty) {
						if (Board.getPiece(square + 2) == Defs::Pieces::empty) {
							addMove(4, 6, Board, ply, 0, 0, false); //White king E1 to G1. Rook move handled by Gameboard::movePiece
						}
					}
				}
			}
		}
	}

	return true;
}

bool MoveGen::whiteQueenSideCastle(Gameboard& Board, int ply)
{
	int square = 4; //E1 white castle starting position
	if (Board.getCastleFlag() & 4)
	{
		if (isInCheck(square, Board, 'w') == false){
			if (isInCheck(square - 1, Board, 'w') == false) {
				if (isInCheck(square - 2, Board, 'w') == false) {
					if (Board.getPiece(square - 1) == Defs::Pieces::empty) {
						if (Board.getPiece(square - 2) == Defs::Pieces::empty) {
							if (Board.getPiece(square - 3) == Defs::Pieces::empty) {
								addMove(4, 2, Board, ply, 0, 0, false); //White king E1 to C1. Rook move handled by Gameboard::movePiece
							}
						}
					}
				}
			}
		}

	}

	return true;
}

bool MoveGen::blackKingSideCastle(Gameboard& Board, int ply)
{
	int square = 116; //E8 black castle starting position
	if (Board.getCastleFlag() & 2)
	{
		if (isInCheck(square, Board, 'b') == false){
			if (isInCheck(square + 1, Board, 'b') == false) {
				if (isInCheck(square + 2, Board, 'b') == false) {
					if (Board.getPiece(square + 1) == Defs::Pieces::empty) {
						if (Board.getPiece(square + 2) == Defs::Pieces::empty) {
							addMove(116, 118, Board, ply, 0, 0, false); //Black king E8 to G8. Rook move handled by Gameboard::movePiece
						}
					}
				}
			}
		}
	}

	return true;
}

bool MoveGen::blackQueenSideCastle(Gameboard& Board, int ply)
{
	int square = 116; //E1 white castle starting position
	if (Board.getCastleFlag() & 1)
	{
		if (isInCheck(square, Board, 'b') == false){
			if (isInCheck(square - 1, Board, 'b') == false) {
				if (isInCheck(square - 2, Board, 'b') == false) {
						if (Board.getPiece(square - 1) == Defs::Pieces::empty) {
							if (Board.getPiece(square - 2) == Defs::Pieces::empty) {
								if (Board.getPiece(square - 3) == Defs::Pieces::empty) {
									addMove(116, 114, Board, ply, 0, 0, false); //Black king E8 to C8. Rook move handled by Gameboard::movePiece
								}
							}
						}
				}
			}
		}

	}

	return true;
}

bool MoveGen::rookMove(Gameboard& Board, int ply, int square, char side, bool capturesOnly)
{
	int rkMoves[4]{ -16,1,-1,16 };
	for (int i = 0; i < 4; i++)
	{
		int squareTo = square + rkMoves[i];
		int b = 0;
		while(b==0) //(Board.getPiece(squareTo) != Defs::Pieces::offboard)
		{
			

			if (Board.onBoard(squareTo) == false) { //off board
				b = 1; 
				continue; 
			}  
			if (Board.getPieceColour((int)Board.getPiece(squareTo)) == side) //occupied by own side
			{
				b = 1;
				continue;
			}
			
			if (Board.getPieceColour((int)Board.getPiece(squareTo)) != side 
				&& Board.getPiece(squareTo) != Defs::Pieces::empty 
				&& Board.getPiece(squareTo) != Defs::Pieces::enpassant
				) //occupied by other side
			{
					addMove(square, squareTo, Board, ply, 0, 0, capturesOnly);
					b = 1;
					continue; 
			}
			if ((Board.getPiece(squareTo) == Defs::Pieces::empty) || (Board.getPiece(squareTo) == Defs::Pieces::enpassant)) //empty square. add move and move to next square
			{
				addMove(square, squareTo, Board, ply, 0, 0, capturesOnly);
				squareTo += rkMoves[i];
			}
			
			
		}

	}
	return true;
}


bool MoveGen::bishopMove(Gameboard& Board, int ply, int square, char side, bool capturesOnly)
{
	int bMoves[4]{ -15,-17,15,17 };
	for (int i = 0; i < 4; i++)
	{
		int squareTo = square + bMoves[i];
		int b = 0;
		while (b == 0) //(Board.getPiece(squareTo) != Defs::Pieces::offboard)
		{
			if (Board.onBoard(squareTo) == false) { //off board
				b = 1;
				continue;
			}
			if (Board.getPieceColour((int)Board.getPiece(squareTo)) == side) //occupied by own side
			{
				b = 1;
				continue;
			}

			if (Board.getPieceColour((int)Board.getPiece(squareTo)) != side &&
				Board.getPiece(squareTo) != Defs::Pieces::empty &&
				Board.getPiece(squareTo) != Defs::Pieces::enpassant) //occupied by other side
			{
					addMove(square, squareTo, Board, ply, 0, 0, capturesOnly);
					b = 1;
					continue; 
			}
			if (Board.getPiece(squareTo) == Defs::Pieces::empty|| Board.getPiece(squareTo) == Defs::Pieces::enpassant) //empty square. add move and move to next square
			{
					addMove(square, squareTo, Board, ply, 0, 0, capturesOnly);
				squareTo += bMoves[i];
			}

		}

	}
	return true;
}

bool MoveGen::whitePawnPromotionForQuiescence(Gameboard& Board, int ply, int square)
{
	//Move 1 square forward
	if (Board.getPiece(square + 16) == Defs::Pieces::empty)
	{
		if ((square + 16) >= 112 && (square + 16) <= 119) //promoted
		{
			addMove(square, (square + 16), Board, ply, 0, static_cast <int>(Defs::Pieces::Q), false);
			addMove(square, (square + 16), Board, ply, 0, static_cast <int>(Defs::Pieces::N), false);
			addMove(square, (square + 16), Board, ply, 0, static_cast <int>(Defs::Pieces::B), false);
			addMove(square, (square + 16), Board, ply, 0, static_cast <int>(Defs::Pieces::R), false);
		}
	}
	return true;
}


bool MoveGen::whitePawnMove(Gameboard &Board, int ply, int square)
{
	//Move 1 square forward
	if (Board.getPiece(square + 16) == Defs::Pieces::empty|| Board.getPiece(square + 16) == Defs::Pieces::enpassant)
	{
		if (( square + 16 ) >= 112 && ( square + 16 ) <= 119) //promoted
		{	
			addMove(square, (square + 16), Board, ply, 0, static_cast <int>(Defs::Pieces::Q),false);
			addMove(square, (square + 16), Board, ply, 0, static_cast <int>(Defs::Pieces::N),false);
			addMove(square, (square + 16), Board, ply, 0, static_cast <int>(Defs::Pieces::B),false);
			addMove(square, (square + 16), Board, ply, 0, static_cast <int>(Defs::Pieces::R),false);
		}
		else
		{
			addMove(square, (square + 16), Board, ply,0, 0,false);
		}
			
			
	}
	//Move 2 squares forward
	//Are we on rank2?
	if(square >= 16 && square <= 23)
	if ((Board.getPiece(square + 32) == Defs::Pieces::empty || Board.getPiece(square + 32) == Defs::Pieces::enpassant)
		&& (Board.getPiece(square + 16) == Defs::Pieces::empty || Board.getPiece(square + 16) == Defs::Pieces::enpassant))
	{
		//addMove(square, (square + 32), Board, (square + 16), false); 
		addMove(square, (square + 32), Board, ply, 1, 0, false);
	}
	return true;
}

bool MoveGen::whitePawnCaptureMove(Gameboard &Board, int ply, int square, bool capturesOnly)
{
	
	if (Board.onBoard(square + 15))
	{
		//black piece on diagonally adjacent squares
		if (Board.getPieceColour((int)Board.getPiece(square + 15)) == 'b')
		{
			
			if (( square + 15 ) >= 112 && ( square + 15 ) <= 119) //promoted
			{
				addMove(square, (square + 15), Board, ply, 0, static_cast <int>(Defs::Pieces::Q), capturesOnly);
				addMove(square, (square + 15), Board, ply, 0, static_cast <int>(Defs::Pieces::N), capturesOnly);
				addMove(square, (square + 15), Board, ply, 0, static_cast <int>(Defs::Pieces::B), capturesOnly);
				addMove(square, (square + 15), Board, ply, 0, static_cast <int>(Defs::Pieces::R), capturesOnly);
			}
			else
			{
				addMove(square, (square + 15), Board, ply, 0, 0,  capturesOnly);
			}
		}
		//en passant capture move
		if ((int)Board.getPiece(square + 15) == (int)Defs::Pieces::enpassant)
		{
			addMove(square, (square + 15), Board, ply, 0, 0, capturesOnly);
		}
	}
	if (Board.onBoard(square + 17))
	{
		if (Board.getPieceColour((int)Board.getPiece(square + 17)) == 'b')
		{
			if (( square + 17 ) >= 112 && ( square + 17 ) <= 119) //promoted
			{
				addMove(square, (square + 17), Board, ply, 0, static_cast <int>(Defs::Pieces::Q), capturesOnly);
				addMove(square, (square + 17), Board, ply, 0, static_cast <int>(Defs::Pieces::N), capturesOnly);
				addMove(square, (square + 17), Board, ply, 0, static_cast <int>(Defs::Pieces::B), capturesOnly);
				addMove(square, (square + 17), Board, ply, 0, static_cast <int>(Defs::Pieces::R), capturesOnly);

			}
			else
			{
				addMove(square, (square + 17), Board, ply, 0, 0, capturesOnly);
			}
		}
		if ((int)Board.getPiece(square + 17) == (int)Defs::Pieces::enpassant)
		{
			addMove(square, (square + 17), Board, ply, 0, 0, capturesOnly);
		}
	}
		return true;
}

bool MoveGen::blackPawnPromotionForQuiescence(Gameboard& Board, int ply, int square)
{
	//Move 1 square forward
	if (Board.getPiece(square - 16) == Defs::Pieces::empty)
	{

		if ((square - 16) >= 0 && (square - 16) <= 7) //promoted
		{
			addMove(square, (square - 16), Board, ply, 0, static_cast <int>(Defs::Pieces::q), false);
			addMove(square, (square - 16), Board, ply, 0, static_cast <int>(Defs::Pieces::n), false);
			addMove(square, (square - 16), Board, ply, 0, static_cast <int>(Defs::Pieces::b), false);
			addMove(square, (square - 16), Board, ply, 0, static_cast <int>(Defs::Pieces::r), false);
		}
		return true;
	}
	return false;
}

bool MoveGen::blackPawnMove(Gameboard& Board, int ply, int square)
{
	//Move 1 square forward
	if (Board.getPiece(square - 16) == Defs::Pieces::empty || Board.getPiece(square - 16) == Defs::Pieces::enpassant)
	{
		
		if (( square - 16 ) >= 0 && ( square - 16 ) <= 7) //promoted
		{
			addMove(square, (square - 16), Board, ply, 0, static_cast <int>(Defs::Pieces::q), false);
			addMove(square, (square - 16), Board, ply, 0, static_cast <int>(Defs::Pieces::n), false);
			addMove(square, (square - 16), Board, ply, 0, static_cast <int>(Defs::Pieces::b), false);
			addMove(square, (square - 16), Board, ply, 0, static_cast <int>(Defs::Pieces::r), false);
		}
		else 
		{
			addMove(square, (square - 16), Board, ply, 0, 0, false);
		}
	}
	//Move 2 squares forward
	//Are we on rank7?
	if (square >= 96 && square <= 103)
		if ((Board.getPiece(square - 32) == Defs::Pieces::empty || Board.getPiece(square - 32) == Defs::Pieces::enpassant)
			&& (Board.getPiece(square - 16) == Defs::Pieces::empty || Board.getPiece(square - 16) == Defs::Pieces::enpassant))
		{
			//addMove(square, (square - 32), Board, (square - 16), false); //sets enpassant flag
			addMove(square, (square - 32), Board, ply, 1, 0, false); //sets enpassant flag
		}
	return true;
}

bool MoveGen::blackPawnCaptureMove(Gameboard& Board, int ply, int square, bool capturesOnly)
{
	
	if (Board.onBoard(square - 15))
	{
		//white piece on diagonally adjacent squares
		if (Board.getPieceColour((int)Board.getPiece(square - 15)) == 'w')
		{
			
			if ((square - 15) >= 0 && (square - 15) <= 7) //promoted
			{
				addMove(square, (square - 15), Board, ply, 0, static_cast <int>(Defs::Pieces::q), capturesOnly);
				addMove(square, (square - 15), Board, ply, 0, static_cast <int>(Defs::Pieces::n), capturesOnly);
				addMove(square, (square - 15), Board, ply, 0, static_cast <int>(Defs::Pieces::b), capturesOnly);
				addMove(square, (square - 15), Board, ply, 0, static_cast <int>(Defs::Pieces::r), capturesOnly);
				
			}
			else
			{
				addMove(square, (square - 15), Board, ply, 0, 0, capturesOnly);
			}
		}
		//en passant flagged piece on diagonally adjacent squares
		if ((int)Board.getPiece(square - 15) == (int)Defs::Pieces::enpassant)
		{
			addMove(square, (square - 15), Board, ply, 0, 0, capturesOnly);
		}
	}
	if (Board.onBoard(square - 17))
	{
		//white piece on diagonally adjacent squares
		if (Board.getPieceColour((int)Board.getPiece(square - 17)) == 'w')
		{
			
			if ((square - 17) >= 0 && (square - 17) <= 7)
			{
				addMove(square, (square - 17), Board, ply, 0, static_cast <int>(Defs::Pieces::q), capturesOnly);
				addMove(square, (square - 17), Board, ply, 0, static_cast <int>(Defs::Pieces::n), capturesOnly);
				addMove(square, (square - 17), Board, ply, 0, static_cast <int>(Defs::Pieces::b), capturesOnly);
				addMove(square, (square - 17), Board, ply, 0, static_cast <int>(Defs::Pieces::r), capturesOnly);

			}
			else
			{
				addMove(square, (square - 17), Board, ply, 0, 0, capturesOnly);
			}
		}
		//en passant flagged piece on diagonally adjacent squares
		if ((int)Board.getPiece(square - 17) == (int)Defs::Pieces::enpassant)
		{
			addMove(square, (square - 17), Board, ply, 0, 0, capturesOnly);
		}
	}
		return true;
}

void MoveGen::printMoveRepresentation(int move)
{
	//Gameboard* Board = Board->getInstance();
	Gameboard Board;

	
	std::cout << endl << "Move representation is:    " << std::bitset<32>(move) << " Integer " << move << endl;
	std::cout << "1 - 7   square from:       " << Board.indexToNotation(fromSquare(move)) <<  " " << fromSquare(move)   << endl;
	std::cout << "8 - 14  square to:         " << Board.indexToNotation(toSquare(move)) <<  " " <<toSquare(move)   << endl;
	string castle;
	if (getMoveCastleFlag(move) & 8) { castle += 'K';}
	if (getMoveCastleFlag(move) & 4) { castle += 'Q'; }
	if (getMoveCastleFlag(move) & 2) { castle += 'k'; }
	if (getMoveCastleFlag(move) & 1) { castle += 'q'; }
	std::cout << "15 - 18 castle:           " << castle << " " << std::bitset<4>(getMoveCastleFlag(move)) << endl;
	if (getCaptured(move) == 0)
	{
		std::cout << "19 - 22 captured piece:    " << "- none" << endl;
	}
	else
	{
		std::cout << "19 - 22 captured piece:    " << getCaptured(move) << " " << Board.pieceToAscii((Defs::Pieces)getCaptured(move)) << endl;
	}
	
	if(getPromoted(move)==0)
	{
		std::cout << "23 - 26 promoted piece:    " << "- none" << endl;
	}
	else
	{
		std::cout << "23 - 26 promoted piece:    " << Board.pieceToAscii((Defs::Pieces)MoveGen::getPromoted(move)) << endl;
	}
	
	if (getMoveEnPassantFlag(move) == 0)
	{
		std::cout << "27 - 27 enpassant flag:    " << getMoveEnPassantFlag(move) << " none" << endl;
	}
	else
	{
		int from = fromSquare(move);
		int to = toSquare(move);

		std::cout << "27 - 27 en passant flag:   "  << getMoveEnPassantFlag(move)  <<  " on square " << (from + to) / 2 << " " << Board.indexToNotation((from + to) / 2) << endl;
	}

	std::cout << endl << "------------------------------------" << endl;
}

//void MoveGen::addMove(int move)
//{
//	moves.push_back(mvlist(move,0,false));
//}

int MoveGen::getCaptureScore(int from, int to, Gameboard& Board, int ply)
{
	int victim = static_cast<int>(Board.getPiece(to));
	int captureScore = 0;
		if (victim > 0)
		{
			int attacker = static_cast<int>(Board.getPiece(from));
			//enpassant capture
			if (victim == static_cast<int>(Defs::Pieces::enpassant) &&
				(attacker == static_cast<int>(Defs::Pieces::p) 
					|| attacker == static_cast<int>(Defs::Pieces::P))
				)
			{
				captureScore = 1000000 + 105; //Pawn taked Pawn is 105
			}
			else
			{
				captureScore = getMvvLvaScores(victim * 14 + attacker) + 1000000;
			
			}
		}
		return captureScore;
}

int MoveGen::getKillerScore(int move, int ply)
{

	if (killers[0][ply] == move) return 900000;
	else if (killers[1][ply] == move) return 800000;
	else return 0;
}

int MoveGen::getHistoryScore(int from, int to, Gameboard& Board)
{
	int score = searchHistory[static_cast <int> (Board.getPiece(from))] [to] ;
	return score;
}

void MoveGen::addMove(int from, int to, Gameboard &Board, int ply, int enpassant, int promoted, bool capturesOnly) {
	//1 - 7   square from
	//8 - 14  square to
	//15 - 18 castle (4 bits)
	//19 - 22 captured piece
	//23 - 29 en passant square
	//30 - 30 promoted piece (1 bit as always reverts to pawn)


	//change to
	//1 - 7   square from
	//8 - 14  square to
	//15 - 18 castle (4 bits)
	//19 - 22 captured piece
	//23 - 26 promoted piece 4 bits repredenting (int) Defs::Pieces qkbrQKBR
	//27 - 27 en passant flag,  square is abs(square to - square from)
	
	
	//square from and square to
	int i = 0;
	int j = 0;
	int k = 0;
	int e = 0;
	i = i + (from & 0x7f);
	j = j + (to & 0x7f);
	int p = (promoted & 0XF); //promoted piece
	
	i = i + (j << 7);
	//captured piece 
	i |= (((int)Board.getPiece(to) & 0xF)  << 18); 

	//en passant flag
	//i = i + (e << 22);
	
	if (enpassant == 1)
	{
		i |= 1u << 26;
	}
	else
	{
		i &= ~(1u << 26);
	}

	//castling flag
	i |= ((Board.getCastleFlag() & 0xF) << 14); //set castle flag from board
	
	//promoted piece
	if (promoted != 0)
	{
		i |= p << 22;
		//i = i + (e << 22);
	}
	 
	int captureScore = getCaptureScore(from, to, Board, ply);
	int killerScore = getKillerScore(i, ply);
	int historyScore = 0;
	if (killerScore == 0) { historyScore = getHistoryScore(from, to, Board); }
	int sortScore = captureScore + killerScore + historyScore;
	
	bool quiet;
	if (captureScore > 0)  quiet = false;
	else quiet = true;

	//if (promoted)
	//{
	//	i |= 1u << 29;
	//} //set the promoted flag to 1
	//else
	//{
	//	i &= ~(1u << 29); //set the promoted flag to 0
	//}
	if (capturesOnly == false)
	{
		moves.push_back(mvlist(i, sortScore, quiet)); 
	}
	else if (capturesOnly == true && captureScore > 0 )
	{
		moves.push_back(mvlist(i, captureScore, quiet));
	}

}

int MoveGen::notationToMoveRepresentation(string move, Gameboard& Board)
{
	//1 - 7   square from
	//8 - 14  square to
	//15 - 18 castle (4 bits)
	//19 - 22 captured piece
	//23 - 26 promoted piece 4 bits repredenting (int) Defs::Pieces qkbrQKBR
	//27 - 27 en passant flag,  square is abs(square to - square from)
	//28 - 31 en passant column
	int from = Board.notationToIndex(move.substr(0, 2));
	int to = Board.notationToIndex(move.substr(2, 2));
	
	//square from and square to
	int i = 0;
	int j = 0;
	int k = 0;
	int e = 0;
	i = i + (from & 0x7f);
	j = j + (to & 0x7f);
	i = i + (j << 7);

	//castling flag
	i |= ((Board.getCastleFlag() & 0xF) << 14); //set castle flag from board

	//captured piece 
	i |= (((int)Board.getPiece(to) & 0xF) << 18);

	if (move.length() >= 5)
	{
		char promotedChar = move.at(4);
		
		//UCI always return promoted piece in lower case
		char colour = Board.getSquareColour(from);
		if (colour == 'w')
		{
			promotedChar = toupper(promotedChar);
		}
		else
		{
			promotedChar = tolower(promotedChar);
		}
		
		int promoted = Board.asciiPieceToInt(promotedChar);
		//Defs::Pieces promotedPiece = static_cast <Defs::Pieces> (Board.asciiPieceToInt(promotedChar));
		//int promoted = static_cast <int>(promotedPiece);

		int p = (promoted & 0XF); //promoted piece
		//promoted piece
		if (promoted != 0)
		{
			i |= p << 22;
		}
	}

	//enpassant flag
	int enpassant = 0;
	if (Board.getPiece(from) == Defs::Pieces::P || Board.getPiece(from) == Defs::Pieces::p)
	{
		if (abs(from - to) == 32)
		{
			enpassant = 1;
		}
	}


	if (enpassant == 1)
	{
		i |= 1u << 26;
	}
	else
	{
		i &= ~(1u << 26);
	}

	return i;
}

string MoveGen::moveRepresentationToNotation(int move)
{
	//Gameboard* Board = Board->getInstance();
	Gameboard Board;
	int f = move & 0x7f;
	int t = ((move >>7) & 0x7f);
	int z = getPromoted(move);
	string from = Board.indexToNotation(f);
	string to = Board.indexToNotation(t);
	char promoted = Board.pieceToAscii(static_cast<Defs::Pieces>(z));  //Todo:: should i tolower() this for UCI?
	string moveString = from + to;
	if (promoted != '-') moveString = moveString + promoted;
	return moveString;
}

int MoveGen::fromSquare(int move)
{
	return (move & 0x7F);
}

int MoveGen::toSquare(int move)
{
	return ((move >>7) & 0x7F);
}

int MoveGen::getCaptured(int move)
{
	return ((move >> 18) & 0xF);
}

void MoveGen::printMoves(Gameboard& Board)
{
	std::cout << std::endl << "Moves: " << std::endl;
	for (int i = 0; i < (int)moves.size(); ++i)
	{
		std::cout << Board.indexToNotation(MoveGen::fromSquare(moves[i].move)) << " "
			<< Board.indexToNotation(MoveGen::toSquare(moves[i].move)) << " "
			<< "Captured:   " << Board.pieceToAscii((Defs::Pieces)MoveGen::getCaptured(moves[i].move)) << " "
			<< "Promoted:  " << Board.pieceToAscii((Defs::Pieces)MoveGen::getPromoted(moves[i].move))
			<< " Sort Score " << moves[i].score 
			<< " Move Integer "<< moves[i].move << std::endl;
	}
	std::cout << endl;
}

void MoveGen::printMove(int move, Gameboard& Board)
{
		std::cout << Board.indexToNotation(MoveGen::fromSquare(move)) << " "
			<< Board.indexToNotation(MoveGen::toSquare(move)) << " "
			<< "Captured:   " << Board.pieceToAscii((Defs::Pieces)MoveGen::getCaptured(move)) << " "
			<< "Promoted:  " << Board.pieceToAscii((Defs::Pieces)MoveGen::getPromoted(move))
			<< " Move Integer " << move << std::endl;
}

vector <MoveGen::moveList> MoveGen::getMoves()
{
	return moves;
}

void MoveGen::initialiseMoves()
{
	moves.clear();
	setAttacksArray();
}

int MoveGen::setMoveCastleFlag(Gameboard& Board, int move)
{
	move &= ~(0x0F << 14); //Clear the castle flags
	move |= (Board.getCastleFlag() << 14); //set the bits in the move integer to the board
	return move;
}

int MoveGen::getMoveCastleFlag(int move)
{
	int castleFlag = ((move >> 14) & 0x0F);
	return castleFlag;
}

void MoveGen::setMoveEnPassantFlag(int& move)
{
	move |= 1u << 26; //set the en passant flag to 1
}

void MoveGen::setMoveEnPassantColumn(int& move, int column)
{
	move &= ~(0x0F << 27); //clear the enpassant column
	move |= (column << 27); //set the enpassant column
}

int MoveGen::getMoveEnPassantColumn(int move)
{
	int enPassantColumn = ((move >> 27) & 0x0F);
	return enPassantColumn;
}

int MoveGen::getMoveEnPassantFlag(int move)
{
	//return ((move >> 22) & 0x7F);
	int enPassantFlag = (1u & (move >> 26));
	return enPassantFlag;
	/*if (enPassantFlag == 1)
	{
		cout << toSquare[move] << endl;
		cout << fromSquare[move] << endl;

		return abs(toSquare(move) - fromSquare(move));
	}
	else { return Defs::NoSquare; };*/
}

void MoveGen::clearMoveEnPassantFlag(int& move)
{
	move &= ~(1u << 26); //set the enPassant flag to 0
}

void MoveGen::setPromoted(int move, int piece)
{
	int p = (piece & 0XF);
	move |= p << 22;
}

int MoveGen::getPromoted(int move)
{
	return ((move >> 22) & 0xF);
}

//==============Attacks=============================
void MoveGen::setAttacksArray()
{
	//unsigned int Attack[257]{ 0 }; //-128 to 128
	int rookMoves[28]{ -16,-32,-48,-64,-80,-96,-112,1,2,3,4,5,6,7,-1,-2,-3,-4,-5,-6,-7, 16,32,48,64,80,96,112 };
	int knightMoves[8]{ 31,33,18,14,-31,-33,-18,-14 };
	//int whitePawnMoves[2]{ 15,17 };
	//int blackPawnMoves[2]{ -15,-17 };
	int whitePawnMoves[2]{ 15,17 };
	int blackPawnMoves[2]{ -15,-17 };

	int bishopMoves[30]{ -15,-30,-45,-60,-75,-90,-105,-120,
						 -17,-34,-51,-68,-85,-102,-119,
						15,30,45,60,75,90,105,120,
						17,34,51,68,85,102,119 };
	int kingMoves[8]{ 1,-1,16,-16,15,17,-15,-17 };

	for (int i = 0; i < 30; i++)
	{

		SET_BIT(Attacks[bishopMoves[i] + 128], (int)Defs::Pieces::b);
		SET_BIT(Attacks[bishopMoves[i] + 128], (int)Defs::Pieces::B);
		SET_BIT(Attacks[bishopMoves[i] + 128], (int)Defs::Pieces::q);
		SET_BIT(Attacks[bishopMoves[i] + 128], (int)Defs::Pieces::Q);
	}


	for (int i = 0; i < 2; i++)
	{
		SET_BIT(Attacks[whitePawnMoves[i] + 128], (int)Defs::Pieces::P);
	}

	for (int i = 0; i < 2; i++)
	{
		SET_BIT(Attacks[blackPawnMoves[i] + 128], (int)Defs::Pieces::p);

	}

	for (int i = 0; i < 28; i++)
	{
		SET_BIT(Attacks[rookMoves[i] + 128], (int)Defs::Pieces::r);
		SET_BIT(Attacks[rookMoves[i] + 128], (int)Defs::Pieces::R);
		SET_BIT(Attacks[rookMoves[i] + 128], (int)Defs::Pieces::q);
		SET_BIT(Attacks[rookMoves[i] + 128], (int)Defs::Pieces::Q);
	}

	for (int i = 0; i < 8; i++)
	{
		SET_BIT(Attacks[knightMoves[i] + 128], (int)Defs::Pieces::n);
		SET_BIT(Attacks[knightMoves[i] + 128], (int)Defs::Pieces::N);
	}
	
	for (int i = 0; i < 8; i++)
	{
		SET_BIT(Attacks[kingMoves[i] + 128], (int)Defs::Pieces::k);
		SET_BIT(Attacks[kingMoves[i] + 128], (int)Defs::Pieces::K);
	}

}

void MoveGen::testAttacks()
{
	cout << " r is " << (int)Defs::Pieces::r << '\n';
	cout << " R is " << (int)Defs::Pieces::R << '\n';
	cout << " q is " << (int)Defs::Pieces::q << '\n';
	cout << " Q is " << (int)Defs::Pieces::Q << '\n';
	cout << "Attacks[1] " << std::bitset<32>(Attacks[1 + 128]) << endl;

	if ( (1 << (int)Defs::Pieces::R) & Attacks[1 + 128]) { cout << "We have R " << '\n'; }
	else { cout << "We don't have R " << '\n'; }

	if ((1 << (int)Defs::Pieces::Q) & Attacks[1 + 128]) { cout << "We have Q " << '\n'; }
	else { cout << "We don't have Q " << '\n'; }

	if ((1 << (int)Defs::Pieces::P) & Attacks[1 + 128]) { cout << "We have P " << '\n'; }
	else { cout << "We don't have P " << '\n'; }

	int kingSq = 2;
	int testSq = 87;
	int test = testSq - kingSq + 128;
	cout << "Attacks[] " << std::bitset<32>(Attacks[test]) << endl;
	//Gameboard* Board = Board->getInstance();
	Gameboard Board;
	for (int i = (int)Defs::Pieces::P; i < (int)Defs::Pieces::k; ++i)
	{
		if ((1 << i) & Attacks[test]) 
		{ 
			cout << Board.pieceToAscii((Defs::Pieces)i); 
		}
	}
	cout << endl;
	
	
}

bool MoveGen::pawnPieceCheck(int kingSquare, Gameboard& Board, int delta, char side)
{
	int i = kingSquare + delta; //e.g delta could be 17, -17 for pawn 
	
		if (Board.onBoard(i) == false) { //off board
			//	cout << "Off board: " << i << endl;
			return false;
		}
		else if (Board.getPieceColour((int)Board.getPiece(i)) == side) { //own colour
			//	cout << "Own colour: " << i << endl;
			return false;
		}

		 
		else if (Board.getPiece(i) == Defs::Pieces::p || Board.getPiece(i) == Defs::Pieces::p)
		{
			if ((1 << (int)Board.getPiece(i)) & Attacks[128 + kingSquare - i]) //in check
			//if ((1 << (int)Board.getPiece(i)) & Attacks[i + 128 - kingSquare]) //in check
			{
				//cout << endl;
				//cout << "Pawn Piece Check: " << i << endl;
				//cout << "Attacks[] " << std::bitset<32>(Attacks[128 + kingSquare - i]) << endl;
				//cout << "Piece     " << std::bitset<32>((int)Board.getPiece(i)) << endl;
				//cout << "Piece (int) " << (int)Board.getPiece(i)<< endl;
				//cout << "Delta " << delta << endl;
				//cout << "Attacking Square " << i << " " << Board.indexToNotation(i) << endl;
				//cout << "King Square " << kingSquare << " " << Board.indexToNotation(kingSquare) << endl;
				return true;
			}
			else if ((1 << (int)Board.getPiece(i)) & ~(Attacks[128 + kingSquare - i])) //not check
			//else if ((1 << (int)Board.getPiece(i)) & ~(Attacks[i + 128 - kingSquare])) //not check
			{
				return false;
			}
		}
		
	return false;
}

bool MoveGen::slidingPieceCheck( int kingSquare, Gameboard& Board, int delta, char side)
	{
		//char kingColour =  Board.getPieceColour((int)Board.getPiece(kingSquare));

		
		
		for (int i = kingSquare + delta; Board.onBoard(i) == true; i += delta)  //e.g delta could be 17, -17 for bishop 
		{
			if (Board.onBoard(i) == false) { //off board
			//	cout << "Off board: " << i << endl;
				break;
			}
			else if (Board.getPieceColour((int)Board.getPiece(i)) == side) { //own colour
			//	cout << "Own colour: " << i << endl;
				break;
			}
			// 

			else if (Board.getPiece(i) == Defs::Pieces::empty) //empty square
			{
			//	cout << "Empty square: " << i << endl;
				continue;
			}
			else if (Board.getPiece(i) == Defs::Pieces::enpassant) //enpassant square
			{
			//	cout << "Enpassant square: " << i << endl;
				continue;
			}
			else if (Board.getPiece(i) == Defs::Pieces::p || Board.getPiece(i) == Defs::Pieces::p) //pawn
			{
				break;
			}

			//else if ((1 << (int)Board.getPiece(i)) & ~(Attacks[128 + kingSquare - i])) //not check
			else if ((1 << (int)Board.getPiece(i)) & ~(Attacks[i + 128 - kingSquare])) //not check
			{
				break;
			}

			//else if ((1 << (int)Board.getPiece(i)) & Attacks[128 + kingSquare - i]) //in check
			else if ((1 << (int)Board.getPiece(i)) & Attacks[i + 128 - kingSquare]) //in check
			{
				//cout << endl;
				//cout << "Sliding Piece Check: " << i << endl;
				//cout << "Attacks[] " << std::bitset<32>(Attacks[128 + kingSquare - i]) << endl;
				//cout << "Piece     " << std::bitset<32>((int)Board.getPiece(i)) << endl;
				//cout << "Piece (int) " << (int)Board.getPiece(i)<< endl;
				//cout << "Delta " << delta << endl;
				//cout << "Attacking Square " << i << " " << Board.indexToNotation(i) << endl;
				//cout << "King Square " << kingSquare << " " << Board.indexToNotation(kingSquare) << endl;
				return true;
			}
		}
		return false;
	}
		
bool MoveGen::knightCheck(int kingSquare, Gameboard& Board, int delta, char side)
 	{
		//char kingColour = Board.getPieceColour((int)Board.getPiece(kingSquare));
			int i = kingSquare + delta;
			
			if (Board.onBoard(i) == false) 
			{ 
			//	cout << "Off board: " << i << endl;
				return false; 
			}  //off board
			else if (Board.getPieceColour((int)Board.getPiece(i)) == side) 
			{
			//	cout << "Own colour: " << i << endl;
				return false; 
			} //occupied by own side
			//else if ((1 << (int)Board.getPiece(i)) & Attacks[128 + kingSquare -i]) //in check
			else if ((1 << (int)Board.getPiece(i)) & Attacks[i + 128 - kingSquare]) //in check
			{

				//cout << "Check: " << i << endl;
				//cout << "Attacks[] " << std::bitset<32>(Attacks[i + 128 - kingSquare]) << endl;
				//cout << "Piece     " << std::bitset<32>((int)Board.getPiece(i)) << endl;
				return true;
			}
			return false;
	}


bool MoveGen::squareAttacked(int square, Gameboard& Board, char side)
	{
		//Gameboard* Board = Board->getInstance();
		//Todo keep this code as useful when i have a piece list
		/* ========================================================================================
		//white pawn
			int whitePawnMoves[2]{ 15,17 };
			for (int i = 0; i < 2; i++)
			{

				if (Board->getSquareColour(square - whitePawnMoves[i]) == Board->getSquareColour(square)) { continue; }
				if( (1 << (int)Board->getPiece(square - whitePawnMoves[i]) & Attacks[whitePawnMoves[i] + 128]) )
				{
					cout << "Square " << square << " " << Board->indexToNotation(square) << " is attacked by white pawn move " << (square - whitePawnMoves[i]) << " " << Board->indexToNotation(square - whitePawnMoves[i]) << endl;
				}
			}
		//black pawn
			int blackPawnMoves[2]{ -15, -17 };
			for (int i = 0; i < 2; i++)
			{

				if (Board->getSquareColour(square - blackPawnMoves[i]) == Board->getSquareColour(square)) { continue; }
				if ((1 << (int)Board->getPiece(square - blackPawnMoves[i]) & Attacks[blackPawnMoves[i] + 128]))
				{
					cout << "Square " << square << " " << Board->indexToNotation(square) << " is attacked by black pawn move " << (square - blackPawnMoves[i]) << " " << Board->indexToNotation(square - whitePawnMoves[i]) << endl;
				}
			}
		==========================================================================================*/
		
		//side is the colour that we are playing, not the opponent

		//knight
		int knMoves[8]{ 31,33,18,14,-31,-33,-18,-14 };
		for (int i = 0; i < 8; i++)
		{
			if (Board.onBoard(square - knMoves[i]) == false) { continue; } //off board
			else if (Board.getSquareColour(square - knMoves[i]) != side && (1 << (int)Board.getPiece(square - knMoves[i]) & Attacks[knMoves[i] + 128]))
			{
				//cout << "Square " << square << " " << Board->indexToNotation(square) << " is attacked by knight " << (square - knMoves[i]) << " " << Board->indexToNotation(square - knMoves[i]) << endl;
				return true;
			}
		}

		//diagonals
		int diagMoves[4]{ -15,-17,15,17 };

		for (int i = 0; i < 4; i++)
		{
			int attack = square - diagMoves[i];
			//if (Board->onBoard(attack) == true) {  //on board
			//cycle through the empty squares
			while ((Board.getPiece(attack) == Defs::Pieces::empty || Board.getPiece(attack) == Defs::Pieces::enpassant) && (Board.onBoard(attack) == true))   //empty
			{
				attack -= diagMoves[i]; //move through empty squares
			}
			if (Board.getSquareColour(attack) != side && Board.onBoard(attack) == true && ((1 << (int)Board.getPiece(attack)) & (Attacks[square - attack + 128])))  //found an attack
			{
				//cout << "Square " << square << " " << Board.indexToNotation(square) << " is attacked by diagonal move " << (attack) << " " << Board.indexToNotation(attack) << endl;
				return true;
			}
		}

		//files
		int fileMoves[2]{ -16,16 };

		for (int i = 0; i < 2; i++)
		{
			int attack = square - fileMoves[i];
			//if (Board->onBoard(attack) == false) { continue; } //off board
			//cycle through the empty squares
			while ((Board.getPiece(attack) == Defs::Pieces::empty || Board.getPiece(attack) == Defs::Pieces::enpassant) && (Board.onBoard(attack) == true))   //empty
			{

				attack -= fileMoves[i]; //move through empty squares
			}
			if (Board.getSquareColour(attack) != side && Board.onBoard(attack) == true && ((1 << (int)Board.getPiece(attack)) & (Attacks[square - attack + 128])))  //found an attack
			{
				//cout << "Square " << square << " " << Board->indexToNotation(square) << " is attacked by file move " << (attack) << " " << Board->indexToNotation(attack) << endl;
				return true;
			}

		}

		//ranks
		int rankMoves[2]{ -1,1 };

		for (int i = 0; i < 2; i++)
		{
			int attack = square - rankMoves[i];
			//if (Board->onBoard(attack) == false) { continue; } //off board
			//cycle through the empty squares
			while ((Board.getPiece(attack) == Defs::Pieces::empty || Board.getPiece(attack) == Defs::Pieces::enpassant) && (Board.onBoard(attack) == true))
			{

				attack -= rankMoves[i]; //move through empty squares
			}
			if (Board.getSquareColour(attack) != side && Board.onBoard(attack) == true && ((1 << (int)Board.getPiece(attack)) & (Attacks[square - attack + 128])))  //found an attack
			{
				//cout << "Square " << square << " " << Board.indexToNotation(square) << " is attacked by rank move " << (attack) << " " << Board.indexToNotation(attack) << endl;
				return true;
			}
		}

		//8 surrounding (king)
		//covered by file ranks and diagonals
		return false;
	}


bool MoveGen::isInCheck(Gameboard& Board, int colour)
	{
		char side;
		if (colour == 1) 
			side = 'w';
		else 
			side = 'b';

		//std::cout << "side is " << side << " and Board.getSide() is " << Board.getSide() << std::endl;
		int kingSquare = Board.getKingSquare(side);
		if (squareAttacked(kingSquare, Board, side) == true)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

bool MoveGen::isInCheck(int kingSquare, Gameboard& Board, char side) //used in move generation enpassant logic
	{
		if (squareAttacked(kingSquare, Board, side) == true)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

/*bool MoveGen::isInCheck(Gameboard& Board)
	{
	char side = Board.getSide();
	int kingSquare = Board.getKingSquare(side);
		if (squareAttacked(kingSquare, Board, side) == true)
	  	{
			return true;
		}
		else
		{
			return false;
		}
	}*/

void MoveGen::initMvvLva() //Most Valuable Victim, Least Valuable Attacker
	{
		int attacker = 0;
		int victim = 0;
		for (attacker = static_cast <int>(Defs::Pieces::P); attacker <= static_cast <int>(Defs::Pieces::k); ++attacker)
			for (victim = static_cast <int>(Defs::Pieces::P); victim <= static_cast <int>(Defs::Pieces::k); ++victim)
				mvvLvaScores[victim * 14 + attacker] = mvvLvaValue[victim] + 6 - (static_cast<int>(mvvLvaValue[attacker]) / 100);
		//So P takes Q is 500 + 6 –(100 / 100) = 505
		//B takes Q is 506 –(300 / 100) = 503
	}

int MoveGen::getMvvLvaScores(int arrayIndex)
	{
		return mvvLvaScores[arrayIndex];
	}

