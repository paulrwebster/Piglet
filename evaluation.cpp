#include "evaluation.h"

int Evaluation::knightMobility(Gameboard& Board, int square)
{
	int side = Board.getPieceColour((int)Board.getPiece(square));
	int bonus = 0;
	int knMoves[8]{ 31,33,18,14,-31,-33,-18,-14 };
	for (int i = 0; i < 8; i++)
	{
		int squareTo = square + knMoves[i];
		if (Board.onBoard(squareTo) == false) { continue; }  //off board
		if (Board.getPieceColour((int)Board.getPiece(squareTo)) == side) 
		{
			bonus += 5; // don't know if 5 is a good number. picked out of the air.
			continue; 
		} //occupied by own side
		if (Board.getPieceColour((int)Board.getPiece(squareTo)) != side &&
			Board.getPiece(squareTo) != Defs::Pieces::empty &&
			Board.getPiece(squareTo) != Defs::Pieces::enpassant) //occupied by other side
			if (Board.getPiece(squareTo) == Defs::Pieces::empty || Board.getPiece(squareTo) == Defs::Pieces::enpassant)
			{
				bonus += Defs::knightMobilityBonus; 
				continue;
			}//empty square. add move and move to next square
	}
	return bonus;
}


int Evaluation::bishopMobility(Gameboard& Board, int square)
{
	int side = Board.getPieceColour((int)Board.getPiece(square));
	int bonus = 0;
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
				bonus += 5; // don't know if 5 is a good number. picked out of the air.
				b = 1;
				continue;
			}

			if (Board.getPieceColour((int)Board.getPiece(squareTo)) != side &&
				Board.getPiece(squareTo) != Defs::Pieces::empty &&
				Board.getPiece(squareTo) != Defs::Pieces::enpassant) //occupied by other side
			{
				b = 1;
				continue;
			}
			if (Board.getPiece(squareTo) == Defs::Pieces::empty || Board.getPiece(squareTo) == Defs::Pieces::enpassant) //empty square. add move and move to next square
			{
				bonus += Defs::bishopMobilityBonus; 
				squareTo += bMoves[i];
			}
		}
	}

	return bonus;
}

int Evaluation::rookMobility(Gameboard& Board, int square)
{
	int side = Board.getPieceColour((int)Board.getPiece(square));
	int bonus = 0;
	int rkMoves[4]{ -16,1,-1,16 };
	for (int i = 0; i < 4; i++)
	{
		int squareTo = square + rkMoves[i];
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
				//add a bonus for defending this piece

				bonus += 5; // don't know if 5 is a good number. picked out of the air.
				continue;
			}

			if (Board.getPieceColour((int)Board.getPiece(squareTo)) != side
				&& Board.getPiece(squareTo) != Defs::Pieces::empty
				&& Board.getPiece(squareTo) != Defs::Pieces::enpassant
				) //occupied by other side
			{
				b = 1;
				continue;
			}
			if ((Board.getPiece(squareTo) == Defs::Pieces::empty) || (Board.getPiece(squareTo) == Defs::Pieces::enpassant)) //empty square. add bonus and move to next square
			{
				//add bonus
				bonus += Defs::rookMobilityBonus; 
				squareTo += rkMoves[i];
			}


		}

	}
	return bonus;
}

int Evaluation::queenMobility(Gameboard& Board, int Square)
{
	int bonus = 0;
	bonus += rookMobility(Board, Square);
	bonus += bishopMobility(Board, Square);
	return bonus;
}

int Evaluation::evaluateBoard(Gameboard& Board) //this version returns +ve white -ve black
{
	int boardScore = 0;

	for (int i = 0; i < 127; ++i)
	{
		boardScore += evaluate(Board.getPiece(i), i, Board); //evaluate square i
	}

	return boardScore;
}

int Evaluation::evaluateBoardNew(Gameboard& Board, bool Print) //this version returns +ve white -ve black. New version with piece mobility
{
	int boardScore = 0;
	rookPairBonus = 0;
	knightPairBonus = 0;
	bishopPairBonus = 0;
	rookMobilityBonus = 0;
	bishopMobilityBonus = 0;
	knightMobilityBonus = 0;
	queenMobilityBonus = 0;
	Defs::GameState gameState = getGameState(Board);

	for (int i = 0; i < 127; ++i)
	{
		Defs::Pieces piece = Board.getPiece(i);
		int eval = evaluate(piece, i, Board); //evaluate square i
		boardScore += eval;
		
		if (piece == Defs::Pieces::R) { rookMobilityBonus += rookMobility(Board, i); }
		if (piece == Defs::Pieces::r) { rookMobilityBonus -= rookMobility(Board, i); }
		if (piece == Defs::Pieces::B) { bishopMobilityBonus += bishopMobility(Board, i); }
		if (piece == Defs::Pieces::b) { bishopMobilityBonus -= bishopMobility(Board, i); }
		if (piece == Defs::Pieces::N) { knightMobilityBonus += knightMobility(Board, i); }
		if (piece == Defs::Pieces::n) { knightMobilityBonus -= knightMobility(Board, i); }
		if (piece == Defs::Pieces::Q && gameState == Defs::GameState::middle) { queenMobilityBonus += queenMobility(Board, i); }
		if (piece == Defs::Pieces::q && gameState == Defs::GameState::middle) { queenMobilityBonus -= queenMobility(Board, i); }
		


		if (Print == true)
		{
			if (eval != 0)
			{
				std::cout << "Piece " << Board.asciiPieces[static_cast<int>(piece)] << " square " << Board.indexToNotation(i) << " Value " << eval << std::endl;
			}
		}
	}
	// Pairs - Bonus fow bishop, penalty for Knight and Rook
	
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::B)] >= 2) bishopPairBonus += 30;
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::b)] >= 2) bishopPairBonus -= 30;
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::N)] >= 2) knightPairBonus -= 8;
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::n)] >= 2) knightPairBonus += 8;
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::R)] >= 2) rookPairBonus -= 16;
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::r)] >= 2) rookPairBonus += 16;

	boardScore += bishopPairBonus;
	boardScore += knightPairBonus;
	boardScore += rookPairBonus;
	boardScore += rookMobilityBonus;
	boardScore += bishopMobilityBonus;
	boardScore += knightMobilityBonus;
	

	if (Print == true)
	{
		std::cout << "Bishop Pair     " << bishopPairBonus << std::endl;
		std::cout << "Knight Pair     " << knightPairBonus << std::endl;
		std::cout << "Rook Pair       " << rookPairBonus << std::endl;
		std::cout << "Rook Mobility   " << rookMobilityBonus << std::endl;
		std::cout << "Bishop Mobility " << bishopMobilityBonus << std::endl;
		std::cout << "Knight Mobility " << knightMobilityBonus << std::endl;
		std::cout << "Evaluation      " << boardScore << std::endl;
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

int Evaluation::printEvaluationNew(Gameboard& Board)
{
	int rv = evaluateBoardNew(Board, true);
	return rv;
}

Defs::GameState Evaluation::getGameState(Gameboard& Board)
{
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::Q)] == 0 && Board.pieceCount[static_cast <int> (Defs::Pieces::q)] == 0)
	{
		return Defs::GameState::end;
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

	if (whiteEndGame && blackEndGame) 
	{ 
		return Defs::GameState::end; 
	} 
	//Every side which has a queen has additionally no other pieces or one minorpiece maximum.

	//Middle Game 
	// Kings to safety, developed most major pieces.
	// The midgame starts when there are 10 or fewer major or minors. Major queen/rook. Minor bishops/knights. 
	// OR the back rank is sparse 
	// OR the white and black pieces are sufficiently mixed on the board.

	//10 or fewer major or minors.
	if (Board.pieceCount[static_cast <int> (Defs::Pieces::q)]
		+ Board.pieceCount[static_cast <int> (Defs::Pieces::r)]
		+ Board.pieceCount[static_cast <int> (Defs::Pieces::b)]
		+ Board.pieceCount[static_cast <int> (Defs::Pieces::n)]
		+ Board.pieceCount[static_cast <int> (Defs::Pieces::Q)]
		+ Board.pieceCount[static_cast <int> (Defs::Pieces::R)]
		+ Board.pieceCount[static_cast <int> (Defs::Pieces::B)]
		+ Board.pieceCount[static_cast <int> (Defs::Pieces::N)] <= 10)
	{
		return Defs::GameState::middle;
	}

	//Both sides castled

	int castleFlag = Board.getCastleFlag();
	if((!castleFlag && 8 || !castleFlag && 4)  && (!castleFlag && 2 || !castleFlag && 1))
	{
		return Defs::GameState::middle;
	}

	return Defs::GameState::start;
}


int Evaluation::evaluate(Defs::Pieces piece, int square, Gameboard& Board) //returns positive for white negative for black
{

	int squareScore = 0;
	//bool endGame = false;
	Defs::GameState gameState = Defs::GameState::start;
	if (piece == Defs::Pieces::K || piece == Defs::Pieces::k)
	{
		gameState = getGameState(Board);
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
		switch (gameState)
		{
		case Defs::GameState::start:
			squareScore += whiteKingMiddleGameEval[square];
			break;
		case Defs::GameState::middle:
			squareScore += whiteKingMiddleGameEval[square];
			break;
		case Defs::GameState::end:
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
		switch (gameState)
		{
		case Defs::GameState::start:
			squareScore -= blackKingMiddleGameEval[square];
			break;
		case Defs::GameState::middle:
			squareScore -= blackKingMiddleGameEval[square];
			break;
		case Defs::GameState::end:
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
