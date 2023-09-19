#include "evaluation.h"


int Evaluation::evaluateBoard(Gameboard& Board) //this version returns +ve white -ve black
{
	int boardScore = 0;

	for (int i = 0; i < 127; ++i)
	{
		boardScore += evaluate(Board.getPiece(i), i, Board); //evaluate square i
	}

	return boardScore;
}

int Evaluation::printEvaluation(Gameboard& Board)
{
	Board.printBoard();
	int boardScore = 0;
	for (int i = 0; i < 127; ++i)
	{
		Defs::Pieces piece = Board.getPiece(i);
		int eval = evaluate(piece, i, Board); //evaluate square i
		boardScore += eval;
		if (eval != 0)
		{
			std::cout << "Piece " << Board.asciiPieces[static_cast<int>(piece)] << " square " << Board.indexToNotation(i) << " Value " << eval << std::endl;
		}
	}
	std::cout << "Evaluation " << boardScore << std::endl;
	return boardScore;
}

bool Evaluation::isEndGame(Gameboard& Board)
{
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::Q)] == 0 && Board.pieceCount[static_cast <int> (Defs::Pieces::q)] == 0)
	{
		return true;
	} //both sides no queen

	bool whiteEndGame = 0;
	bool blackEndGame = 0;
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::Q)] >= 1 &&
		(
			Board.pieceCount[static_cast <int> (Defs::Pieces::N)] +
			Board.pieceCount[static_cast <int> (Defs::Pieces::B)] +
			Board.pieceCount[static_cast <int> (Defs::Pieces::R)] <= 1
			)
		)
		whiteEndGame = true;
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::Q)] == 0)
	{
		whiteEndGame = true;
	}

	if (Board.pieceCount[static_cast <int> (Defs::Pieces::q)] >= 1 &&
		(
			Board.pieceCount[static_cast <int> (Defs::Pieces::n)] +
			Board.pieceCount[static_cast <int> (Defs::Pieces::b)] +
			Board.pieceCount[static_cast <int> (Defs::Pieces::r)] <= 1
			)
		)
		blackEndGame = true;
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::q)] == 0)
	{
		blackEndGame = true;
	}

	if (whiteEndGame && blackEndGame) { return true; } //Every side which has a queen has additionally no other pieces or one minorpiece maximum.

	return false;
}


int Evaluation::evaluate(Defs::Pieces piece, int square, Gameboard& Board) //returns positive for white negative for black
{

	int squareScore = 0;
	bool endGame = false;
	if (piece == Defs::Pieces::K || piece == Defs::Pieces::k)
	{
		endGame = isEndGame(Board);
		int debug = 0;
	}
	switch (piece)
	{
		//White Pieces 
	case Defs::Pieces::P:
		squareScore += pieceVal[(int)piece];
		squareScore += whitePawnEval[square];
		break;
	case Defs::Pieces::N:
		squareScore += pieceVal[(int)piece];
		squareScore += whiteKnightEval[square];
		break;
	case Defs::Pieces::B:
		squareScore += pieceVal[(int)piece];
		squareScore += whiteBishopEval[square];
		break;
	case Defs::Pieces::R:
		squareScore += pieceVal[(int)piece];
		squareScore += whiteRookEval[square];
		break;
	case Defs::Pieces::Q:
		squareScore += pieceVal[(int)piece];
		squareScore += whiteQueenEval[square];
		break;
	case Defs::Pieces::K:
		squareScore += pieceVal[(int)piece];
		switch (endGame)
		{
		case false:
			squareScore += whiteKingMiddleGameEval[square];
			break;
		case true:
			squareScore += whiteKingEndGameEval[square];
			break;
		default:
			break;
		}
		break;
		//Black pieces 
	case Defs::Pieces::p:
		squareScore -= pieceVal[(int)piece];
		squareScore -= blackPawnEval[square];
		break;
	case Defs::Pieces::n:
		squareScore -= pieceVal[(int)piece];
		squareScore -= blackKnightEval[square];
		break;
	case Defs::Pieces::b:
		squareScore -= pieceVal[(int)piece];
		squareScore -= blackBishopEval[square];
		break;
	case Defs::Pieces::r:
		squareScore -= pieceVal[(int)piece];
		squareScore -= blackRookEval[square];
		break;
	case Defs::Pieces::q:
		squareScore -= pieceVal[(int)piece];
		squareScore -= blackQueenEval[square];
		break;
	case Defs::Pieces::k:
		squareScore -= pieceVal[(int)piece];
		switch (endGame)
		{
		case false:
			squareScore -= blackKingMiddleGameEval[square];
			break;
		case true:
			squareScore -= blackKingEndGameEval[square];
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return squareScore;
}


int Evaluation::getPieceVal(int pieceIndex)
{
	return pieceVal[pieceIndex];
}
