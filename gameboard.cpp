#include "gameboard.h"
#include "evaluation.h"
#include "moveGen.h"
#include "hashing.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip> //for setw
#include <cstdlib> //for atoi
#include <bitset>
#include <sstream>


Gameboard::Gameboard()
{
	//std::cout << "Calling Gameboard constructor " << std::endl;
}

Gameboard::~Gameboard()
{
	//std::cout << "Calling Gameboard destructor " << std::endl;
}


void Gameboard::movePiece(int& move,  Hashing& Hash, MoveGen& Moves)
{
	int epSquare = Defs::NoSquare;
	int from = Moves.fromSquare(move);
	int to = Moves.toSquare(move);
	int promotedPiece = Moves.getPromoted(move);

	Defs::Pieces pieceMoving = getPiece(from);
	Defs::Pieces pieceCaptured = static_cast<Defs::Pieces>(Moves.getCaptured(move));
	Defs::Pieces piecePromoted = static_cast<Defs::Pieces>(Moves.getPromoted(move));
	int pieceMovingCount = pieceCount[static_cast<int>(pieceMoving)];
	int pieceCapturedCount = pieceCount[static_cast<int>(pieceCaptured)];
	int pieceMovingIndex = 0;
	int pieceCapturedIndex = 0;
	int pieceCapturedLastIndex = 0;
	int piecePromotedIndex = 0;
	int piecePromotedLastIndex = 0;
	int pieceMovingLastIndex = 0;



	//get hold of the existing oppostion ep square on the board
	int oldEPSquare = epRemovePieceList(); //removes ep from piecelist and reduces count
	int oldEPColumn = 0;
	if (oldEPSquare != 0)
	{
		oldEPColumn = indexToFile(oldEPSquare) + 1; // column 1 to 8
		Moves.setMoveEnPassantColumn(move, oldEPColumn); //updates the move with the column of the ep that we have removed
	}

	//remove old ep from Zobrist
	if (oldEPSquare != 0)
	{
		Hash.setZobristEp(oldEPSquare);
	}

	//find the index of the piece moving
	for (int count = 1; count <= pieceMovingCount; ++count)
	{
		if (pieceList[static_cast<int>(pieceMoving) * 10 + count] == from)
		{
			pieceMovingIndex = static_cast<int>(pieceMoving) * 10 + count;
			break;
		}
	}

		//find the index of the piece captured
	for (int count = 1; count <= pieceCapturedCount; ++count)
	{
		if (pieceList[static_cast<int>(pieceCaptured) * 10 + count] == to)
		{
			pieceCapturedIndex = static_cast<int>(pieceCaptured) * 10 + count;
			break;
		}

	}

	if (pieceCaptured == Defs::Pieces::empty)  //don't want to overflow the array with empty square references
	{
		pieceCapturedCount = 0;
		pieceCapturedIndex = 0;
	}

	//set index for piece captured to the square for last piece in the array for that piece
			//find the index, in the array, of the last  similar piece to that captured
	if (pieceCapturedIndex != 0 && static_cast<int>(pieceCaptured) != 14) //deal with ep(14) later
	{
		pieceCapturedLastIndex = static_cast<int>(pieceCaptured) * 10 + pieceCapturedCount;
		pieceList[pieceCapturedIndex] = pieceList[pieceCapturedLastIndex];
		//Empty last piece[index] for captured piece
		pieceList[pieceCapturedLastIndex] = Defs::NoSquare;
		//reduce count for captured piece
		pieceCount[static_cast<int>(pieceCaptured)]--;
		//reduce material for the captured piece
		int colour = getPieceColourInt(static_cast<int>(pieceCaptured)); //we need its colour 
		//Evaluation eval;
		material[colour] -= pieceVal[static_cast<int>(pieceCaptured)];
	}

	//Take off bypassed en passant pawn
	if (getPiece(from) == Defs::Pieces::P
		&& pieceCaptured == Defs::Pieces::enpassant
		&& (to - from == 15 || to - from == 17))
	{
		pieceBoard[to - 16] = Defs::Pieces::empty; //clear the taken pawn

		//update the piece list
		//===========================================================
		pieceCaptured = Defs::Pieces::p;
		pieceCapturedCount = pieceCount[static_cast<int>(pieceCaptured)];
		//find the index of the piece captured
		for (int count = 1; count <= pieceCapturedCount; ++count)
		{
			if (pieceList[static_cast<int>(pieceCaptured) * 10 + count] == (to - 16))
			{
				pieceCapturedIndex = static_cast<int>(pieceCaptured) * 10 + count;
				break;
			}
		}


		//find the index, in the array, of the last  similar piece to that captured
		pieceCapturedLastIndex = static_cast<int>(pieceCaptured) * 10 + pieceCapturedCount;
		//set index for piece captured to the square for last piece in the array for that piece
		pieceList[pieceCapturedIndex] = pieceList[pieceCapturedLastIndex];
		//Empty last piece[index] for captured piece
		pieceList[pieceCapturedLastIndex] = static_cast<int>(Defs::NoSquare);
		//reduce the count for the captured piece
		pieceCount[static_cast<int>(pieceCaptured)]--;
		//reduce material for the captured piece
		int colour = getPieceColourInt(static_cast<int>(pieceCaptured)); //we need its colour 
		//Evaluation eval;
		material[colour] -= pieceVal[static_cast<int>(pieceCaptured)];

		//===========end of piecelist stuff================================

		//Transposition table 
		// At end for function Hash.setZobristPiece(to, pieceCaptured) will xor pawn from ep square
		//rather than pawn square. Need to do an xor here to correct
		Hash.setZobristPiece(to - 16, pieceCaptured);
		Hash.setZobristPiece(to, pieceCaptured);

	}

	if (getPiece(from) == Defs::Pieces::p
		&& pieceCaptured == Defs::Pieces::enpassant
		&& (to - from == -15 || to - from == -17))
	{

		pieceBoard[to + 16] = Defs::Pieces::empty;
		//update the piece list
		pieceCaptured = Defs::Pieces::P;
		pieceCapturedCount = pieceCount[static_cast<int>(pieceCaptured)];
		//find the index of the piece captured
		for (int count = 1; count <= pieceCapturedCount; ++count)
		{
			if (pieceList[static_cast<int>(pieceCaptured) * 10 + count] == (to + 16))
			{
				pieceCapturedIndex = static_cast<int>(pieceCaptured) * 10 + count;
				break;
			}
		}
		//find the index, in the array, of the last  similar piece to that captured
		pieceCapturedLastIndex = static_cast<int>(pieceCaptured) * 10 + pieceCapturedCount;
		//set index for piece captured to the square for last piece in the array for that piece
		pieceList[pieceCapturedIndex] = pieceList[pieceCapturedLastIndex];
		//Empty last piece[index] for captured piece
		pieceList[pieceCapturedLastIndex] = static_cast<int>(Defs::NoSquare);
		//reduce the count for the captured piece
		pieceCount[static_cast<int>(pieceCaptured)]--;
		//reduce material for the captured piece
		int colour = getPieceColourInt(static_cast<int>(pieceCaptured)); //we need its colour 
		//Evaluation eval;
		material[colour] -= pieceVal[static_cast<int>(pieceCaptured)];

		//Transposition table 
		// At end for function Hash.setZobristPiece(to, pieceCaptured) will xor pawn from ep square
		//rather than pawn square. Need to do an xor here to correct
		Hash.setZobristPiece(to + 16, pieceCaptured);
		Hash.setZobristPiece(to, pieceCaptured);

	}
	//at this point we have left opposition ep on the piece board    

	//get the location of the new enpassant square from move integer 
	if (Moves.getMoveEnPassantFlag(move) == 1)
	{
		epSquare = (from + to) / 2;
		pieceCount[static_cast <int>(Defs::Pieces::enpassant)]++;
		int epPiece = static_cast<int>(Defs::Pieces::enpassant);
		int count = pieceCount[epPiece];
		pieceList[epPiece * 10 + count] = epSquare;
	}
	if (epSquare > 0 && epSquare != Defs::NoSquare)
	{
		pieceBoard[epSquare] = Defs::Pieces::enpassant;  //set the enpassant pseudo piece on the bypassed square
	}

	//promotion stuff
	if (piecePromoted == Defs::Pieces::empty)
	{
		//move the moving piece to the "to" square
		pieceList[pieceMovingIndex] = to;
	}
	else //promoted
	{
		//add promoted piece to array
		pieceList[(static_cast <int> (piecePromoted) * 10) + pieceCount[static_cast<int>(piecePromoted)] + 1] = to;

		//increase count for promoted piece
		pieceCount[static_cast<int>(piecePromoted)]++;
		//increase material for promoted piece
		int colour = pieceColourInt[static_cast<int>(piecePromoted)];
		material[colour] += pieceVal[static_cast<int>(piecePromoted)];
		//get index in array for last pawn same as promoted pawn
		pieceMovingLastIndex = static_cast<int>(pieceMoving) * 10 + pieceMovingCount;
		//move last piece in array for similar pawn to promoted pawn index
		pieceList[pieceMovingIndex] = pieceList[pieceMovingLastIndex];
		//Empty last piece[index] for promoted pawn
		pieceList[pieceMovingLastIndex] = static_cast<int>(Defs::NoSquare);
		//reduce count for promoted pawn
		pieceCount[static_cast<int>(pieceMoving)]--;
		//reduce material for promoted pawn. 
		material[colour] -= pieceVal[static_cast<int>(pieceMoving)];

		//Hash stuff
		Hash.setZobristPiece(to, pieceMoving);
		Hash.setZobristPiece(to, piecePromoted);

	}
	// =======================End of pieceList stuff====================================

	//King Squares
	if (getPiece(from) == Defs::Pieces::K)
	{
		whiteKingSquare = to;
	}
	if (getPiece(from) == Defs::Pieces::k)
	{
		blackKingSquare = to;
	}

	//remove the oppostion ep piece from the piece board
	char player = getSide();
	if (player == 'w')
	{
		clearEnpassantFlag('b');
	}
	else
	{
		clearEnpassantFlag('w');
	}


	//Castling
	//Piece taken is rook
	if (to == 0) // R
	{
		if (castleFlag & 4) {
			setCastleFlag(-4);
			Hash.setZobristCastle(4); //xor 4
		}
	}

	if (to == 7) // R 
	{
		if (castleFlag & 8) {
			setCastleFlag(-8);
			Hash.setZobristCastle(8);
		}
	}

	if (to == 112) // r
	{
		if (castleFlag & 1) {
			setCastleFlag(-1);
			Hash.setZobristCastle(1);
		}
	}

	if (to == 119) // r
	{
		if (castleFlag & 2) {
			setCastleFlag(-2);
			Hash.setZobristCastle(2);
		}
	}

	//rook and king moves - amend castle flags

	if (from == 0 || from == 4) // R or K
	{
		if (castleFlag & 4) {
			setCastleFlag(-4);
			Hash.setZobristCastle(4); //xor 4
		}
	}

	if (from == 7 || from == 4) // R or K
	{
		if (castleFlag & 8) {
			setCastleFlag(-8);
			Hash.setZobristCastle(8);
		}
	}


	if (from == 112 || from == 116) // r or k
	{
		if (castleFlag & 1) {
			setCastleFlag(-1);
			Hash.setZobristCastle(1);
		}
	}

	if (from == 119 || from == 116) // r or k
	{
		if (castleFlag & 2) {
			setCastleFlag(-2);
			Hash.setZobristCastle(2);
		}
	}


	if (from == 4 && to == 6)
	{
		if (getPiece(from) == Defs::Pieces::K) //castling white short
		{
			pieceBoard[notationToIndex("f1")] = pieceBoard[notationToIndex("h1")];//Rook  h1f1
			pieceBoard[notationToIndex("h1")] = Defs::Pieces::empty;
			//Zobrist Rook  h1f1
			Hash.setZobristPiece(7, Defs::Pieces::R);
			Hash.setZobristPiece(5, Defs::Pieces::R);


			//update piecelist
			//====================================================
			//get index of rook
			pieceMoving = Defs::Pieces::R;
			pieceMovingCount = pieceCount[static_cast<int>(pieceMoving)]; //decrement as zero index
			//find the index of the rook moving
			pieceMovingIndex = 0;
			for (int count = 1; count <= pieceMovingCount; ++count)
			{
				if (pieceList[static_cast<int>(pieceMoving) * 10 + count] == 7)   
				{
					pieceMovingIndex = static_cast<int>(pieceMoving) * 10 + count;
					break;
				}
			}
			//move the moving piece to the "to" square
			if (pieceMovingIndex != 0)
			{
				pieceList[pieceMovingIndex] = 5; //f1
			}
			//===============end of piecelist stuff ============
			//reset pieceMoving to what it was for from and to Zobrist stuff
			pieceMoving = Defs::Pieces::K;
		}
	}
	if (from == 4 && to == 2)
	{
		if (getPiece(from) == Defs::Pieces::K) //castling white long
		{
			pieceBoard[notationToIndex("d1")] = pieceBoard[notationToIndex("a1")];//Rook  a1d1
			pieceBoard[notationToIndex("a1")] = Defs::Pieces::empty;
			//Zobrist Rook a1 d1
			Hash.setZobristPiece(0, Defs::Pieces::R);
			Hash.setZobristPiece(3, Defs::Pieces::R);

			//update pieceboard
			//get index of rook
			pieceMoving = Defs::Pieces::R;
			pieceMovingCount = pieceCount[static_cast<int>(pieceMoving)];
			//find the index of the rook moving
			pieceMovingIndex = 0;
			for (int count = 1; count <= pieceMovingCount; count++)
			{
				if (pieceList[static_cast<int>(pieceMoving) * 10 + count] == 0)   //a1 
				{
					pieceMovingIndex = static_cast<int>(pieceMoving) * 10 + count;
					break;
				}
			}
			//move the moving piece to the "to" square
			if (pieceMovingIndex != 0)
			{
				pieceList[pieceMovingIndex] = 3; //d1
			}
			//reset pieceMoving to what it was for from and to Zobrist stuff
			pieceMoving = Defs::Pieces::K;
		}
	}
	if (from == 116 && to == 118)
	{
		if (getPiece(from) == Defs::Pieces::k) //castling black short
		{
			pieceBoard[notationToIndex("f8")] = pieceBoard[notationToIndex("h8")];//Rook  h8f8
			pieceBoard[notationToIndex("h8")] = Defs::Pieces::empty;
			//Zobrist rook h8 f8
			Hash.setZobristPiece(119, Defs::Pieces::r);
			Hash.setZobristPiece(117, Defs::Pieces::r);

			//update pieceboard
			//=================
			//get index of rook
			pieceMoving = Defs::Pieces::r;
			pieceMovingCount = pieceCount[static_cast<int>(pieceMoving)];
			//find the index of the rook moving
			pieceMovingIndex = 0;
			for (int count = 1; count <= pieceMovingCount; count++)
			{
				if (pieceList[static_cast<int>(pieceMoving) * 10 + count] == 119)   //h8 
				{
					pieceMovingIndex = static_cast<int>(pieceMoving) * 10 + count;
					break;
				}
			}
			//move the moving piece to the "to" square
			if (pieceMovingIndex != 0)
			{
				pieceList[pieceMovingIndex] = 117; //f8
			}
			//reset pieceMoving to what it was for from and to Zobrist stuff
			pieceMoving = Defs::Pieces::k;
		}

	}
	if (from == 116 && to == 114)
	{
		if (getPiece(from) == Defs::Pieces::k) //castling white short
		{
			pieceBoard[notationToIndex("d8")] = pieceBoard[notationToIndex("a8")];//Rook  a8d8
			pieceBoard[notationToIndex("a8")] = Defs::Pieces::empty;
			//Zobrist rook d8 a8
			Hash.setZobristPiece(112, Defs::Pieces::r);
			Hash.setZobristPiece(115, Defs::Pieces::r);

			//update pieceboard
			//=================
			//get index of rook
			pieceMoving = Defs::Pieces::r;
			pieceMovingCount = pieceCount[static_cast<int>(pieceMoving)];
			//find the index of the rook moving
			pieceMovingIndex = 0;
			for (int count = 1; count <= pieceMovingCount; count++)
			{
				if (pieceList[static_cast<int>(pieceMoving) * 10 + count] == 112)   //h8 
				{
					pieceMovingIndex = static_cast<int>(pieceMoving) * 10 + count;
					break;
				}
			}
			//move the moving piece to the "to" square
			if (pieceMovingIndex != 0)
			{
				pieceList[pieceMovingIndex] = 115; //f8
			}
			//reset pieceMoving to what it was for from and to Zobrist stuff
			pieceMoving = Defs::Pieces::k;
		}
	}

	//Zobrist
	//xor out from square
	Hash.setZobristPiece(from, pieceMoving);
	//xor in to square
	Hash.setZobristPiece(to, pieceMoving);
	//xor out captured piece
	if (pieceCaptured != Defs::Pieces::enpassant) //hashed out the enpassant square earlier in function after epRemovePieceList()
	{
		Hash.setZobristPiece(to, pieceCaptured);
	}
		
	if (epSquare != Defs::NoSquare) Hash.setZobristEp(epSquare);
		
	pieceBoard[to] = pieceBoard[from];
	pieceBoard[from] = Defs::Pieces::empty;

	//Promotion
	if (promotedPiece != static_cast <int> (Defs::Pieces::empty))
	{
		pieceBoard[to] = static_cast <Defs::Pieces>(promotedPiece);
	}

	setBoardHash(Hash.getZobristKey());
	Hash.setZobristSide();
	swapSide();

}

	void Gameboard::revertPiece(int& move,  Hashing & Hash, MoveGen & Moves)
	{
		Hash.setZobristSide();
		swapSide();
		int from = Moves.fromSquare(move);
		int to = Moves.toSquare(move);
		int captured = Moves.getCaptured(move);
		int epFlag = Moves.getMoveEnPassantFlag(move);
		int epSquare = Defs::NoSquare;
		if (epFlag == 1)
			epSquare = (from + to) / 2;

		//clear our own enpassant square from the piece list
		if (epSquare > 0 && epSquare != Defs::NoSquare)
		{
			int oldEPSquare = epRemovePieceList(); //removes ep from piecelist and reduces count
		}
		//update the piece list
		//===============================================================================================
		Defs::Pieces pieceMoving = getPiece(to);
		Defs::Pieces pieceCaptured = static_cast<Defs::Pieces>(captured);
		int pieceMovingCount = pieceCount[static_cast<int>(pieceMoving)];
		int pieceCapturedCount = pieceCount[static_cast<int>(pieceCaptured)]; //decrement?
		int pieceMovingIndex = 0;
		int pieceCapturedIndex = 0;
		int pieceCapturedLastIndex = 0;

		Defs::Pieces piecePromoted = static_cast<Defs::Pieces>(Moves.getPromoted(move));
		int piecePromotedCount = pieceCount[static_cast<int>(piecePromoted)]; //decrement as zero index
		int piecePromotedIndex = 0;
		int piecePromotedLastIndex = 0;
		Defs::Pieces pawnPromoted = Defs::Pieces::empty;

		pieceBoard[from] = pieceBoard[to]; //put moving piece back where it was
		
		 //find the index of the piece that moved
		for (int count = 1; count <= pieceMovingCount; ++count)
		{
			if (pieceList[static_cast<int>(pieceMoving) * 10 + count] == to)
			{
				pieceMovingIndex = static_cast<int>(pieceMoving) * 10 + count;
				break;
			}
		}

		if (piecePromoted == Defs::Pieces::empty)
		{
			//move the moving piece to the "from" square
			pieceList[pieceMovingIndex] = from;
		}
		else //promoted
		{

			//where the promoted piece is in the piece list gets jiggled around when pieces are captured, so have to find it
			//set index for piece promoted to the square for last piece in the array for that piece
			//find the index, in the array, of the last  similar piece to that promoted

			//find the piece promoted index
			for (int i = 1; i <= 10; ++i)
			{
				if (pieceList[static_cast<int>(piecePromoted) * 10 + i] == to)
				{
					piecePromotedIndex = static_cast<int>(piecePromoted) * 10 + i;
					break;
				}
			}
			piecePromotedLastIndex = static_cast<int>(piecePromoted) * 10 + pieceCount[static_cast<int>(piecePromoted)];
			//put piecePromoted last in list
			int a = piecePromotedIndex;
			int b = piecePromotedLastIndex;
			int c = pieceList[piecePromotedIndex];
			int d = pieceList[piecePromotedLastIndex];
			pieceList[a] = d;
			pieceList[b] = c;



			pieceList[pieceCapturedIndex] = pieceList[pieceCapturedLastIndex];
			//Empty last piece[index] for captured piece
			pieceList[pieceCapturedLastIndex] = Defs::NoSquare;

			//decrease material for promoted piece
			int colour = pieceColourInt[static_cast<int>(piecePromoted)];  //0 white 1 black 2 other 
			//material[colour] -= eval.getPieceVal(static_cast<int>(piecePromoted));
			material[colour] -= pieceVal[static_cast<int>(piecePromoted)];
			//remove promoted piece from array
			pieceList[piecePromotedLastIndex] = static_cast<int>(Defs::Pieces::empty);
			//decrease count for promoted piece
			pieceCount[static_cast<int>(piecePromoted)]--;

			//get pawn that was promoted;
			if (colour == 0) pawnPromoted = Defs::Pieces::P;
			else if (colour == 1) pawnPromoted = Defs::Pieces::p;
			//increment the count
			pieceCount[static_cast<int>(pawnPromoted)]++;
			//add pawn to array
			pieceList[static_cast<int>(pawnPromoted) * 10 + pieceCount[static_cast<int>(pawnPromoted)]] = from;
			//increment the material
			material[colour] += pieceVal[static_cast<int>(pawnPromoted)];

			//Hash stuff
			//Piece moving is set to qbrn rather than p. Need to adjust this for the pawn and the promoted piece
			Hash.setZobristPiece(from, pawnPromoted);
			Hash.setZobristPiece(from, piecePromoted);

		}


		//find the index of the piece captured. Get the last one from count and increment
		pieceCapturedIndex = 0;
		pieceCapturedIndex = static_cast<int>(pieceCaptured) * 10 + pieceCapturedCount + 1;

		if (pieceCaptured == Defs::Pieces::empty)  //don't want to overflow the array with empty square references
		{
			pieceCapturedCount = 0;
			pieceCapturedIndex = 0;
		}

		if (pieceCaptured == Defs::Pieces::enpassant)  //ep has its own logic 
		{
			pieceCapturedCount = 0;
			pieceCapturedIndex = 0;
		}

		//set index for piece captured to the square for last piece in the array for that piece
		if (pieceCapturedIndex != 0) //???
		{
			//find the index of the piece captured. Get the last one from count and increment(i.e. don't decrement)
		    //increase count for captured piece
			pieceCount[static_cast<int>(pieceCaptured)]++;
			//increase material for the captured piece
			int colour = getPieceColourInt(static_cast<int>(pieceCaptured)); //we need its colour 
			//Evaluation eval;
			material[colour] += pieceVal[static_cast<int>(pieceCaptured)];
			//Restore sqaure value of piece captured
			pieceList[pieceCapturedIndex] = to;
		}
		// =======================End of pieceList stuff====================================
						
		whiteKingSquare = getKingSquare('w');
		blackKingSquare = getKingSquare('b');

		if (epSquare > 0 && epSquare != Defs::NoSquare)
		{
			pieceBoard[epSquare] = Defs::Pieces::empty;  //reverse the set of the enpassant pseudo piece on the bypassed square
		}


		//add back old ep from pieceList
		int oldEPcolumn = Moves.getMoveEnPassantColumn(move);
		string oldEPRank;
		string oldEPNotation;
		char player = getSide();
		if (player == 'w') { oldEPRank = "6"; }
		else { oldEPRank = "3"; }
		switch (oldEPcolumn)
		{
		case 1:
			oldEPNotation = "a" + oldEPRank;
			break;
		case 2:
			oldEPNotation = "b" + oldEPRank;
			break;
		case 3:
			oldEPNotation = "c" + oldEPRank;
			break;
		case 4:
			oldEPNotation = "d" + oldEPRank;
			break;
		case 5:
			oldEPNotation = "e" + oldEPRank;
			break;
		case 6:
			oldEPNotation = "f" + oldEPRank;
			break;
		case 7:
			oldEPNotation = "g" + oldEPRank;
			break;
		case 8:
			oldEPNotation = "h" + oldEPRank;
			break;
		default:
			oldEPNotation = "NoEP";
		}
		int oldEPSquare;
		if (oldEPNotation == "NoEP")
		{
			oldEPSquare = Defs::NoSquare;
		}
		else
		{
			oldEPSquare = notationToIndex(oldEPNotation);
		}

		if (oldEPSquare > 0 && oldEPSquare != Defs::NoSquare)
		{
			epAddPieceList(oldEPSquare);
		}


		if (oldEPSquare > 0 && oldEPSquare != Defs::NoSquare)
		{
			pieceBoard[oldEPSquare] = Defs::Pieces::enpassant;  //set the enpassant pseudo piece on the bypassed square
		}

		//remove old ep from Zobrist
		if (oldEPSquare > 0 && oldEPSquare != Defs::NoSquare)
		{
			Hash.setZobristEp(oldEPSquare);
		}

		int colour = getPieceColourInt(static_cast<int>(piecePromoted)); //promoted piece 0 empty is colour 2
		if (colour == 0)
		{
			pieceBoard[from] = Defs::Pieces::P;
		} //white
		else if (colour == 1)
		{
			pieceBoard[from] = Defs::Pieces::p;
		} //black

		//Restore the opposition's bypassed en passant pawn
		if (getPiece(from) == Defs::Pieces::P
			&& captured == (int)Defs::Pieces::enpassant
			&& (to - from == 15 || to - from == 17))
		{
			pieceBoard[to - 16] = Defs::Pieces::p;
			pieceBoard[to] = Defs::Pieces::enpassant;

			//update the piece list
			//===========================================================
			pieceCaptured = Defs::Pieces::p;
			pieceCapturedCount = pieceCount[static_cast<int>(pieceCaptured)] + 1; //increment as adding back pawn
			//find the index of the piece captured
			pieceCapturedIndex = static_cast<int>(pieceCaptured) * 10 + pieceCapturedCount;

			if (pieceCapturedIndex != 0) //???
			{
				//increase count for captured piece
				pieceCount[static_cast<int>(pieceCaptured)]++;
				//increase material for the captured piece
				int colour = getPieceColourInt(static_cast<int>(pieceCaptured)); //we need its colour 
				//Evaluation eval;
				material[colour] += pieceVal[static_cast<int>(pieceCaptured)];
				//Restore sqaure value of piece captured
				pieceList[pieceCapturedIndex] = to - 16;
			}
			//===========end of piecelist stuff================================

			//Transposition table 
			// At end for function Hash.setZobristPiece(to, pieceCaptured) will xor pawn from ep square
			//rather than pawn square. Need to do an xor here to correct
			Hash.setZobristPiece(to - 16, pieceCaptured);
			Hash.setZobristPiece(to, pieceCaptured);

		}
		else if (getPiece(from) == Defs::Pieces::p
			&& captured == (int)Defs::Pieces::enpassant
			&& (to - from == -15 || to - from == -17))
		{
			pieceBoard[to + 16] = Defs::Pieces::P;
			pieceBoard[to] = Defs::Pieces::enpassant;
			//update the piece list
			//===========================================================
			pieceCaptured = Defs::Pieces::P;
			pieceCapturedCount = pieceCount[static_cast<int>(pieceCaptured)] + 1; //increment as adding back pawn
			//find the index of the piece captured
			pieceCapturedIndex = static_cast<int>(pieceCaptured) * 10 + pieceCapturedCount;

			if (pieceCapturedIndex != 0) //???
			{
				//increase count for captured piece
				pieceCount[static_cast<int>(pieceCaptured)]++;
				//increase material for the captured piece
				int colour = getPieceColourInt(static_cast<int>(pieceCaptured)); //we need its colour 
				//Evaluation eval;
				material[colour] += pieceVal[static_cast<int>(pieceCaptured)];
				//Restore sqaure value of piece captured
				pieceList[pieceCapturedIndex] = to + 16;
			}
			//===========end of piecelist stuff================================

			//Transposition table 
			// At end for function Hash.setZobristPiece(to, pieceCaptured) will xor pawn from ep square
			//rather than pawn square. Need to do an xor here to correct
			Hash.setZobristPiece(to + 16, pieceCaptured);
			Hash.setZobristPiece(to, pieceCaptured);


		}
		else
		{
			pieceBoard[to] = (Defs::Pieces)captured;
		}



		//Castling
		//find out how castleFlag has changed

		int cFlagDiff = (castleFlag ^ Moves.getMoveCastleFlag(move)); //XOR
		if (cFlagDiff & 8) { Hash.setZobristCastle(8); }
		if (cFlagDiff & 4) { Hash.setZobristCastle(4); }
		if (cFlagDiff & 2) { Hash.setZobristCastle(2); }
		if (cFlagDiff & 1) { Hash.setZobristCastle(1); }
		clearCastleFlag();
		setCastleFlag(Moves.getMoveCastleFlag(move)); //revert the castle flag from the one archived to the move integer


		if (from == 4 && to == 6)
		{
			if (getPiece(from) == Defs::Pieces::K) //castling white short
			{
				pieceBoard[notationToIndex("h1")] = pieceBoard[notationToIndex("f1")];//Reverse rook  h1f1
				pieceBoard[notationToIndex("f1")] = Defs::Pieces::empty;

				//update piecelist
				//====================================================
				//get index of rook
				pieceMoving = Defs::Pieces::R;
				pieceMovingCount = pieceCount[static_cast<int>(pieceMoving)];

				//Zobrist Rook  h1f1
				Hash.setZobristPiece(7, Defs::Pieces::R);
				Hash.setZobristPiece(5, Defs::Pieces::R);

				//find the index of the rook moving
				pieceMovingIndex = 0;
				for (int count = 1; count <= pieceMovingCount; ++count)
				{
					if (pieceList[static_cast<int>(pieceMoving) * 10 + count] == 5)   //f1 
					{
						pieceMovingIndex = static_cast<int>(pieceMoving) * 10 + count;
						break;
					}
				}
				//move the moving piece to the "to" square
				if (pieceMovingIndex != 0)
				{
					pieceList[pieceMovingIndex] = 7; //h1
				}

				//===============end of piecelist stuff ============
				//reset pieceMoving to what it was for from and to Zobrist stuff
				pieceMoving = Defs::Pieces::K;

			}
		}
		if (from == 4 && to == 2)
		{
			if (getPiece(from) == Defs::Pieces::K) //castling white long
			{
				pieceBoard[notationToIndex("a1")] = pieceBoard[notationToIndex("d1")];//Reverse rook  a1d1
				pieceBoard[notationToIndex("d1")] = Defs::Pieces::empty;

				//Zobrist Rook a1 d1
				Hash.setZobristPiece(0, Defs::Pieces::R);
				Hash.setZobristPiece(3, Defs::Pieces::R);


				//update piecelist
				//====================================================
				//get index of rook
				pieceMoving = Defs::Pieces::R;
				pieceMovingCount = pieceCount[static_cast<int>(pieceMoving)];
				//find the index of the rook moving
				pieceMovingIndex = 0;
				for (int count = 1; count <= pieceMovingCount; ++count)
				{
					if (pieceList[static_cast<int>(pieceMoving) * 10 + count] == 3)   //d1 
					{
						pieceMovingIndex = static_cast<int>(pieceMoving) * 10 + count;
						break;
					}
				}
				//move the moving piece to the "to" square
				if (pieceMovingIndex != 0)
				{
					pieceList[pieceMovingIndex] = 0; //a1
				}
				//===============end of piecelist stuff ============
				//reset pieceMoving to what it was for from and to Zobrist stuff
				pieceMoving = Defs::Pieces::K;
			}
		}
		if (from == 116 && to == 118)
		{
			if (getPiece(from) == Defs::Pieces::k) //castling black short
			{
				pieceBoard[notationToIndex("h8")] = pieceBoard[notationToIndex("f8")];//Reverse rook h8f8
				pieceBoard[notationToIndex("f8")] = Defs::Pieces::empty;

				//Zobrist rook h8 f8
				Hash.setZobristPiece(119, Defs::Pieces::r);
				Hash.setZobristPiece(117, Defs::Pieces::r);

				//update piecelist
				//====================================================
				//get index of rook
				pieceMoving = Defs::Pieces::r;
				pieceMovingCount = pieceCount[static_cast<int>(pieceMoving)];
				//find the index of the rook moving
				pieceMovingIndex = 0;
				for (int count = 1; count <= pieceMovingCount; ++count)
				{
					if (pieceList[static_cast<int>(pieceMoving) * 10 + count] == 117)   //f8 
					{
						pieceMovingIndex = static_cast<int>(pieceMoving) * 10 + count;
						break;
					}
				}
				//move the moving piece to the "to" square
				if (pieceMovingIndex != 0)
				{
					pieceList[pieceMovingIndex] = 119; //h8
				}

				//===============end of piecelist stuff ============
				 //reset pieceMoving to what it was for from and to Zobrist stuff
				pieceMoving = Defs::Pieces::k;
			}
		}
		if (from == 116 && to == 114)
		{
			if (getPiece(from) == Defs::Pieces::k) //castling black short
			{
				pieceBoard[notationToIndex("a8")] = pieceBoard[notationToIndex("d8")];//Reverse rook  a8d8
				pieceBoard[notationToIndex("d8")] = Defs::Pieces::empty;

				//Zobrist rook d8 a8
				Hash.setZobristPiece(112, Defs::Pieces::r);
				Hash.setZobristPiece(115, Defs::Pieces::r);

				//update piecelist
				//====================================================
				//get index of rook
				pieceMoving = Defs::Pieces::r;
				pieceMovingCount = pieceCount[static_cast<int>(pieceMoving)];
				//find the index of the rook moving
				pieceMovingIndex = 0;
				for (int count = 1; count <= pieceMovingCount; ++count)
				{
					if (pieceList[static_cast<int>(pieceMoving) * 10 + count] == 115)   //d8 
					{
						pieceMovingIndex = static_cast<int>(pieceMoving) * 10 + count;
						break;
					}
				}
				//move the moving piece to the "to" square
				if (pieceMovingIndex != 0)
				{
					pieceList[pieceMovingIndex] = 112; //a8
				}

				//===============end of piecelist stuff ============
				//reset pieceMoving to what it was for from and to Zobrist stuff
				pieceMoving = Defs::Pieces::k;

			}
		}
		//Zobrist
		//xor out from square
		Hash.setZobristPiece(from, pieceMoving);
		//xor in to square
		Hash.setZobristPiece(to, pieceMoving);
		//xor out captured piece
		if (pieceCaptured != Defs::Pieces::enpassant) //hashed out the enpassant square earlier in function after epRemovePieceList()
		{
			Hash.setZobristPiece(to, pieceCaptured);
		}
		//xor in ep file
		if (epSquare != Defs::NoSquare) Hash.setZobristEp(epSquare);
		setBoardHash(Hash.getZobristKey());
	}

int Gameboard::epRemovePieceList()
{
	int sq = 0;
	int piece = 0;
	piece = static_cast<int>(Defs::Pieces::enpassant);
	int count = pieceCount[piece];
	if (count > 0)
	{
		sq = pieceList[piece * 10 + count];
		pieceList[piece * 10 + count] = 0;
		pieceCount[piece]--;
	}
	return sq;
}

void Gameboard::epAddPieceList(int square)
{
	int piece = 0;
	piece = static_cast<int>(Defs::Pieces::enpassant);
	pieceCount[piece]++;
	int count = pieceCount[piece];
	if (count > 0)
	{
		pieceList[piece * 10 + count] = square;
	}
}

char Gameboard::getSide()
{
	return side;
}

int Gameboard::getSideInt()
{
	if (side == 'w')
	{
		return 1;
	}
	else
	{
		return -1;
	};
}

void Gameboard::setSide(char side)
{
	side = side;
}
char Gameboard::getPieceColour(int piece)
{
	//if (piece < 0) { std::cout << "piece is " << piece << std::endl;  return 'x'; }
	//if (piece >= sizeof(pieceColour)) { std::cout << "piece is " << piece << std::endl; return 'x'; }
	return pieceColour[piece];
}

int Gameboard::getPieceColourInt(int piece)
{
	return pieceColourInt[piece];
}

char Gameboard::getSquareColour(int square)
{
	if (square >= 0 && square <= 127)
	{
		return pieceColour[(int)pieceBoard[square]];
	}
	else
	{
		return '-';
	}

}


int Gameboard::getCastleFlag()
{
	return castleFlag;
}

int Gameboard::setCastleFlag(int flag)
{
	castleFlag += flag;
	return castleFlag;
}

int Gameboard::clearCastleFlag()
{
	castleFlag = 0;
	return castleFlag;
}

int Gameboard::parseFen(string fen)
{
	int rank = 7;
	int file = -1;
	int count = 0;
	initBoard();
	int fenCnt = 0;
	int i;
	int side = 0;


	//Output the fenstring as a space delimited stream and load a vector with the parts 
	vector<string> fenParts;
	string buf;
	istringstream iss{ fen };
	while (iss >> buf) {
		fenParts.push_back(buf);
	}


	//Parse the pieces
	while (fenCnt < (int)fenParts.at(0).length())
	{
		file++;
		i = notationToIndex(file, rank);
		switch (fenParts.at(0)[fenCnt])
		{
		case '8': count = 7;
			pieceBoard[i] = Defs::Pieces::empty;
			for (int j = 0; j < count; ++j)
			{
				file++;
				i = notationToIndex(file, rank);
				pieceBoard[i] = Defs::Pieces::empty;
			}
			fenCnt++;
			break;
		case '7': count = 6;
			pieceBoard[i] = Defs::Pieces::empty;
			for (int j = 0; j < count; ++j)
			{
				file++;
				i = notationToIndex(file, rank);
				pieceBoard[i] = Defs::Pieces::empty;
			}
			fenCnt++;
			break;
		case '6': count = 5;
			pieceBoard[i] = Defs::Pieces::empty;
			for (int j = 0; j < count; ++j)
			{
				file++;
				i = notationToIndex(file, rank);
				pieceBoard[i] = Defs::Pieces::empty;
			}
			fenCnt++;
			break;
		case '5': count = 4;
			pieceBoard[i] = Defs::Pieces::empty;
			for (int j = 0; j < count; ++j)
			{
				file++;
				i = notationToIndex(file, rank);
				pieceBoard[i] = Defs::Pieces::empty;
			}
			fenCnt++;
			break;
		case '4': count = 3;
			pieceBoard[i] = Defs::Pieces::empty;

			for (int j = 0; j < count; ++j)
			{
				file++;
				i = notationToIndex(file, rank);
				pieceBoard[i] = Defs::Pieces::empty;

			}
			fenCnt++;
			break;
		case '3': count = 2;
			pieceBoard[i] = Defs::Pieces::empty;

			for (int j = 0; j < count; ++j)
			{
				file++;
				i = notationToIndex(file, rank);
				pieceBoard[i] = Defs::Pieces::empty;

			}
			fenCnt++;
			break;
		case '2': count = 1;
			pieceBoard[i] = Defs::Pieces::empty;

			for (int j = 0; j < count; ++j)
			{
				file++;
				i = notationToIndex(file, rank);
				pieceBoard[i] = Defs::Pieces::empty;

			}
			fenCnt++;
			break;
		case '1': count = 0;
			pieceBoard[i] = Defs::Pieces::empty;

			for (int j = 0; j < count; ++j)
			{
				file++;
				i = notationToIndex(file, rank);
				pieceBoard[i] = Defs::Pieces::empty;

			}
			fenCnt++;
			break;
		case 'P':
			pieceBoard[i] = Defs::Pieces::P;
			fenCnt++;
			break;
		case 'N':
			pieceBoard[i] = Defs::Pieces::N;
			fenCnt++;
			break;
		case 'B':
			pieceBoard[i] = Defs::Pieces::B;
			fenCnt++;
			break;
		case 'R':
			pieceBoard[i] = Defs::Pieces::R;
			fenCnt++;
			break;
		case 'Q':
			pieceBoard[i] = Defs::Pieces::Q;
			fenCnt++;
			break;
		case 'K':
			pieceBoard[i] = Defs::Pieces::K;
			//whiteKingSquare = i;
			fenCnt++;
			break;
		case 'p':
			pieceBoard[i] = Defs::Pieces::p;
			fenCnt++;
			break;
		case 'n':
			pieceBoard[i] = Defs::Pieces::n;
			fenCnt++;
			break;
		case 'b':
			pieceBoard[i] = Defs::Pieces::b;
			fenCnt++;
			break;
		case 'r':
			pieceBoard[i] = Defs::Pieces::r;
			fenCnt++;
			break;
		case 'q':
			pieceBoard[i] = Defs::Pieces::q;
			fenCnt++;
			break;
		case 'k':
			pieceBoard[i] = Defs::Pieces::k;
			//blackKingSquare = i;
			fenCnt++;
			break;
		case '/':  //moving to the next rank
			rank--;
			file = -1;
			fenCnt++;
			// printBoard();
			std::cout << endl;
			break;
		default:
			break;
		}
	}

	//Parse the colour
	if (fenParts.size() >= 2) {
		Gameboard::side = fenParts.at(1).at(0);
		side = fenParts.at(1).at(0);
	}


	//Parse the castle flags
	if (fenParts.size() >= 3) {
		Gameboard::castleFlag = 0;
		for (i = 0; i < (int)fenParts.at(2).length(); i++)
		{
			switch (fenParts.at(2)[i])
			{
			case 'K':
				Gameboard::castleFlag += 8;
				break;
			case 'Q':
				Gameboard::castleFlag += 4;
				break;
			case 'k':
				Gameboard::castleFlag += 2;
				break;
			case 'q':
				Gameboard::castleFlag += 1;
				break;
			default:
				break;
			}
		}
	}

	//En passant
	if (fenParts.size() >= 4)
	{
		if (fenParts.at(3).at(0) != '-')
		{
			int enPassant = notationToIndex(fenParts.at(3).at(0), fenParts.at(3).at(1));
			pieceBoard[enPassant] = Defs::Pieces::enpassant;
		}
	}

	//Halfmove clock
	if (fenParts.size() >= 5)
	{
		halfMove = stoi(fenParts.at(4));
	}

	//Fullmove number
	if (fenParts.size() >= 6)
	{
		fullMove = stoi(fenParts.at(5));
	}
	//set the king squares
	setKingSquares();
	if (side == 'w') return 1;
	else if (side == 'b') return -1;
	else return 0;
}

void Gameboard::initSquare()
{
	int index;
	for (index = 0; index < 128; ++index)
	{
		square[index] = index;

	}
}

void Gameboard::initBoard()
{
	for (int i = 0; i < 128; ++i)
	{
		if (onBoard(i) == true)
		{
			pieceBoard[i] = Defs::Pieces::empty;
		}
		else
		{
			pieceBoard[i] = Defs::Pieces::offboard;
		}
	}
	Gameboard::castleFlag = 0; //1111
	Gameboard::side = 'w';
}

void Gameboard::printBoard() {


	for (int r = 7; r >= 0; --r)
	{
		std::cout << rank[r] << "| ";

		for (int f = 0; f < 8; ++f)
		{
			Defs::Pieces pieces = pieceBoard[notationToIndex(f, r)];
			std::cout << asciiPieces[static_cast<int>(pieces)] << ' ';
		}

		std::cout << endl;
	}
	std::cout << "  ----------------" << endl;
	std::cout << "   ";
	for (int f = 0; f < 8; ++f)
	{
		std::cout << file[f] << " ";
	}
	std::cout << endl;
	std::cout << "Side: " << side << endl;
	std::cout << "Castle Flag: " << std::bitset<4>(castleFlag) << std::endl;
	std::cout << "Halfmove Clock: " << std::dec << halfMove << std::endl;
	std::cout << "Fullmove Number: " << std::dec << fullMove << std::endl;
	Evaluation Eval;
	//int eval = Eval.evaluateBoard(*this);
	//std::cout << "Board evaluation: " << eval << std::endl << std::endl;

}

unsigned int Gameboard::rankAlgabraicToInt(char rank)
{
	for (int i = 0; i < 8; ++i)
	{
		if (Gameboard::rank[i] == rank)
		{
			return i;
		}
	}
	return 8;
}

unsigned int Gameboard::fileAlgabraicToInt(char file)
{
	for (int i = 0; i < 8; ++i)
	{
		if (Gameboard::file[i] == file)
		{
			return i;
		}
	}
	return 8;
}

unsigned int Gameboard::notationToIndex(char file, char rank)
{
	int f = fileAlgabraicToInt(file);
	int r = rankAlgabraicToInt(rank);
	int index = (r * 16) + f;
	return index;
}

unsigned int Gameboard::notationToIndex(std::string notation)
{
	int f = fileAlgabraicToInt(notation[0]);
	int r = rankAlgabraicToInt(notation[1]);
	int index = (r * 16) + f;
	return index;
}

unsigned int Gameboard::notationToIndex(int file, int rank)
{
	//int f = fileAlgabraicToInt(file);
	//int r = rankAlgabraicToInt(rank);
	int index = (rank * 16) + file;
	return index;
}

int Gameboard::indexToFile(int index)
{
	int file = (index & 7);
	return file;
}
string Gameboard::indexToNotation(int index)
{
	if (index <= 128)
	{
		int f = (index & 7);
		int r = (index >> 4);
		char file = Gameboard::file[f];
		char rank = Gameboard::rank[r];
		string s;
		s.push_back(file);
		s.push_back(rank);
		return s;
	}
	else
	{
		return "-";
	}
}

bool Gameboard::onBoard(int index)
{
	if (index & 0x88)
	{
		return false;
	}
	else
	{
		return true;
	}
}

Defs::Pieces Gameboard::getPiece(int square)
{
	return pieceBoard[square];
}

void Gameboard::setPiece(int square, Defs::Pieces piece)
{
	if (square != Defs::NoSquare)
	{
		pieceBoard[square] = piece;
	}
}

char Gameboard::pieceToAscii(Defs::Pieces p)
{
	return asciiPieces[(int)p];
}

int Gameboard::asciiPieceToInt(char piece)
{
	auto it = std::find(std::begin(asciiPieces), std::end(asciiPieces), piece);
	if (it != std::end(asciiPieces))
	{
		int index = std::distance(asciiPieces, it);
		return index;
	}
	return 0;
}

void Gameboard::setKingSquares()  //Todo replace with piecelist code
{
	whiteKingSquare = getPieceSquare(static_cast<int>(Defs::Pieces::K), 1);
	blackKingSquare = getPieceSquare(static_cast<int>(Defs::Pieces::k), 1);
	/*
	for (int i = 0; i < 128; ++i)
	{
			if (getPiece(i) == Defs::Pieces::K)
			{
				whiteKingSquare = i;
			}

			else if (getPiece(i) == Defs::Pieces::k)
			{
				blackKingSquare = i;
			}
	}
	*/
}

int Gameboard::getKingSquare(char side)
{
	if (side == 'w')
	{
		if (pieceCount[6] == 1)
		{
			return pieceList[61];
		}
		else
		{
			return Defs::NoSquare;
		}
	}
	else
		if (pieceCount[12] == 1)
		{
			{ return pieceList[121]; }
		}
		else
		{
			return Defs::NoSquare;
		}
}

//Piece list stuff
int Gameboard::PieceIndex(int piece, int pieceCount) //Return the index of a piece in piece list
{
	if (pieceCount == 0)
	{
		return Defs::NoSquare;
	}
	else
	{
		return piece * 10 + pieceCount;
	}
}

void Gameboard::printPieceList()
{
	int piece, pieceCnt;
	for (piece = static_cast<int> (Defs::Pieces::empty); piece <= static_cast<int> (Defs::Pieces::enpassant); ++piece)
	{
		if (piece == static_cast<int>(Defs::Pieces::offboard)) continue;
		std::cout << "Piece " << asciiPieces[piece] << " Count " << pieceCount[piece] << " On Squares ";
		for (pieceCnt = 1; pieceCnt <= pieceCount[piece]; ++pieceCnt)
		{
			std::cout << indexToNotation(pieceList[PieceIndex(piece, pieceCnt)]) << " "; // << pieceList[PieceIndex(piece, pieceCnt)] << " ";
		}
		std::cout << std::endl;
	}
}
void Gameboard::printMaterial()
{
	std::cout << "White Material " << material[0] << std::endl;
	std::cout << "Black Material " << material[1] << std::endl;
}

void Gameboard::updatePieceList()
{
	Evaluation eval;

	int piece = 0, sq = 0, index = 0, colour = 0;

	for (index = 0; index < 16 * 10; ++index)
	{
		//pieceList[index] = static_cast<int>(Defs::Pieces::empty);
		pieceList[index] = static_cast<int>(Defs::NoSquare);
	}
	for (index = 0; index < 3; ++index)
	{
		material[index] = 0;
	}
	for (index = 0; index < 15; ++index) //used to count the number of each piece
	{
		pieceCount[index] = 0;
	}

	for (index = 0; index < 127; ++index) //loop through all the squares
	{
		sq = index;
		piece = static_cast<int>(pieceBoard[sq]); //get the piece
		if (piece >= static_cast<int>(Defs::Pieces::P) && piece <= static_cast<int>(Defs::Pieces::k) || piece == static_cast<int>(Defs::Pieces::enpassant))//if the piece isn't empty or offboard
		{
			colour = pieceColourInt[piece]; //we need its colour 
			material[colour] += eval.getPieceVal(piece); //as we are going to increment the material value for that colour
			pieceCount[piece]++; //increment the count of that piece
			pieceList[piece * 10 + pieceCount[piece]] = sq; //fill in the piece list
		}
	}


}
int Gameboard::getPieceColour(Defs::Pieces piece)
{
	return pieceColourInt[static_cast<int>(piece)];
}

int Gameboard::getPieceSquare(int piece, int count)
{
	return pieceList[piece * 10 + count];
}

int Gameboard::clearEnpassantFlag(char side)
{
	if (side == 'w') // If white making move, clear all white enpassant squares
	{
		for (int i = 32; i <= 39; ++i) //squares A3 to H3
		{
			if (pieceBoard[i] == Defs::Pieces::enpassant)
			{
				pieceBoard[i] = Defs::Pieces::empty;
				return i;
			}
		}
	}
	else if (side == 'b') // If black making move, clear all black enpassant squares
	{
		for (int i = 80; i <= 87; ++i) //squares A6 to H6
		{
			if (pieceBoard[i] == Defs::Pieces::enpassant)
			{
				pieceBoard[i] = Defs::Pieces::empty;
				return i;
			}
		}
	}
	return 0;
}

void Gameboard::setBoardHash(unsigned long long boardHash)
{
	hash = boardHash;
}
unsigned long long Gameboard::getBoardHash()
{
	return hash;
}

char Gameboard::swapSide()
{
	if (side == 'w')
		side = 'b';
	else
		side = 'w';
	return side;
}


