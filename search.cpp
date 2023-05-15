#include "search.h"
#include <chrono> 
#include <cstring>

MoveGen chessMoves;
Evaluation Eval;
//Hashing Hash;
extern int killers[2][Defs::MaxSearchDepth];
extern int searchHistory[15][128]; //[number of pieces][number of squares]
extern chrono::steady_clock::time_point start;

void Search::stopEngine()
{
	stop = true;
}
void Search::startEngine()
{
	stop = false;
}

void Search::clearBestMove()
{
	bestMove = Defs::NoMove;
}

void Search::clearPreviousBestMove()
{
	previousBestMove = Defs::NoMove;
}

void Search::clearBestLine()
{
	bestLine.cmove = 0;
	for (int i = 0; i < Defs::MaxSearchDepth; ++i)
	{
		bestLine.argmove[i] = Defs::NoMove;
	}
}

void Search::clearPreviousBestLine()
{
	previousBestLine.cmove = 0;
	for (int i = 0; i < Defs::MaxSearchDepth; ++i)
	{
		previousBestLine.argmove[i] = Defs::NoMove;
	}
}

void Search::clearSearch()
{
	//reset long long nodes used in checkmate logic
	nodes = 0;
	
	//clear killers
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < (Defs::MaxSearchDepth); ++j)
		{
			killers[i][j] = 0;
		}
	}

	//fail high
	failHigh = 0;
	failHighFirst = 0;
	failQHigh = 0;
	failQHighFirst = 0;


	//clear searchHistory
	for (int i = 0; i < 15; ++i)
		for (int j = 0; j < 128; ++j)
		{
			searchHistory[i][j] = 0;
		}

	//time start and bool stop
	startEngine();
}

void Search::resetFailHigh()
{
	failHigh = 0;
	failHighFirst = 0;
	failQHigh = 0;
	failQHighFirst = 0;
}

int Search::getFailHigh()
{
	return failHigh;
}

int Search::getFailHighFirst()
{
	return failHighFirst;
}

int Search::getFailQHigh()
{
	return failQHigh;
}

int Search::getFailQHighFirst()
{
	return failQHighFirst;
}

int Search::getBestMoveFound()
{
	//class scoped instance variable set at minimax top-level recursive call
	return bestMove;
}

int Search::getPreviousBestMove()
{
	//class scoped instance variable set at minimax top-level recursive call
	return previousBestMove;
}

void
Search::setPreviousBestMove(int bestMove)
{
	//class scoped instance variable set at minimax top-level recursive call
	previousBestMove = bestMove;
}


Search::LINE Search::getBestLine()
{
	return bestLine;
}

Search::LINE Search::getPreviousBestLine()
{
	return previousBestLine;
}
void Search::setPreviousBestLine(Search::LINE bestLine)
{
	previousBestLine = bestLine;
}

void Search::setBestMoveDepth(int depth)
{
	bestMoveDepth = depth;
}

size_t Search::getNumberOfNodes()
{
	return numberOfNodes;
}

size_t Search::getNumberOfQNodes()
{
	return numberOfQNodes;
}

void Search::resetNumberOfNodes()
{
	numberOfNodes = 0;
	numberOfQNodes = 0;
}

size_t Search::getNumberOfHashMatches()
{
	return numberOfHashMatches;
}

size_t Search::getNumberOfQHashMatches()
{
	return numberOfQHashMatches;
}

void Search::resetNumberOfHashMatches()
{
	numberOfHashMatches = 0;
}

void Search::resetNumberOfQHashMatches()
{
	numberOfQHashMatches = 0;
}



void Search::initMoveGenMvvLva()
{
	chessMoves.initMvvLva();
}
int Search::reorderMoves(int bestMove, vector <MoveGen::moveList>* moves) //reorder best move first for iterative deepening
{

	vector<MoveGen::moveList>::iterator it;
	int best = getBestMoveFound();
	if (best == 0) return 0;
	it = std::find(moves->begin(), moves->end(), best); //find the best move in the vector
	//it = std::find_if(moves->begin(), moves->end(), [&b = best](const MoveGen::moveList& m) {return m.move == b; }); //lambda function

	if (it != moves->end())
	{
		long long i = std::distance(moves->begin(), it);
		int j = 0;
		std::swap(moves->at(i), moves->at(j));

		//string bestMoveNotation = Moves.moveRepresentationToNotation(moves->at(0));
		//std::cout << "First move in vector is "  << bestMoveNotation << std::endl;
		return 1;
	}
	else
	{
		return 0;
	}


}

long long Search::perft(Gameboard& Board, Hashing& Hash, int depth)
{
	int colour = Board.getSideInt();
	long long nodes = 0;
	vector <MoveGen::moveList> moves;
	int n_moves = 0;
	int i = 0;

	if (depth == 0)
	{
		return 1;  //return 1 for each leaf node to add to nodes total
	}

	moves = chessMoves.generateMoves(Board, depth);

	for (int i = 0; i < moves.size(); i++)
	{
		Board.movePiece(moves[i].move, Hash, chessMoves);

		if (chessMoves.isInCheck(Board, colour) == false)
		{
			nodes += perft(Board, Hash, depth - 1);
		}
		Board.revertPiece(moves[i].move, Hash, chessMoves);
	}


	return nodes;
}


long long Search::divide(Gameboard& Board, Hashing& Hash, int depth)
{
	long long nodes = 0;
	vector <MoveGen::moveList> moves;
	long long prev_nodes = 0;
	long long cur_nodes = 0;
	int i = 0;
	int colour = Board.getSideInt();


	if (depth == 0)
	{
		return 1;  //return 1 for each leaf node to add to nodes total
	}
	moves = chessMoves.generateMoves(Board, depth);

	for (int i = 0; i < moves.size(); i++)
	{

		Board.movePiece(moves[i].move, Hash, chessMoves);
		prev_nodes = nodes;

		//if (Moves.isInCheck(Board, Board.getSide()) == false)
		if (chessMoves.isInCheck(Board, colour) == false)
		//if (Moves.isInCheck(Board) == false)
		{
			nodes += perft(Board, Hash, depth - 1);

		}

		Board.revertPiece(moves[i].move, Hash, chessMoves);


		cur_nodes = nodes - prev_nodes;
		std::cout << chessMoves.moveRepresentationToNotation(moves[i].move) << " " << cur_nodes << " " << moves[i].move << std::endl;

	}

	return nodes;
}


long long Search::divideCheckHash(Gameboard& Board, Hashing& Hash, Hashing& CheckHash, int depth)
{
	long long nodes = 0;
	vector <MoveGen::moveList> moves;
	long long prev_nodes = 0;
	long long cur_nodes = 0;
	int i = 0;
	int colour = Board.getSideInt();
	

	if (depth == 0)
	{
		//perftLeafNodes++;
		return 1;  //return 1 for each leaf node to add to nodes total
	}

	moves = chessMoves.generateMoves(Board, depth);

	for (int i = 0; i < moves.size(); i++)
	{
		Board.movePiece(moves[i].move, Hash, chessMoves);
		prev_nodes = nodes;

		//std::cout << "After move " << Moves.moveRepresentationToNotation(moves[i]) << std::endl;
		//Board.printBoard();

		//include this code to check that Zobrist keys match up
		if (Hash.getZobristKey() != CheckHash.generatePositionKey(Board))
		{
			std::cout << "Divide moves " << i << std::endl;
			std::cout << "Zobrist mismatch on Move " << moves[i].move << " " << chessMoves.moveRepresentationToNotation(moves[i].move) << std::endl;
			std::cout << "Generated Hash Key is       : " << CheckHash.generatePositionKey(Board) << std::endl; //quick and inefficient to see if works
			std::cout << "Board Hash Key is           : " << Hash.getZobristKey() << std::endl;
			//std::cout << "Board Generated Hash Key is : " << Hash.generatePositionKey(Board) << std::endl;
		}
		if (chessMoves.isInCheck(Board, colour) == false)
		{
			//	if (moves[i] == 256740)
			//	{
			//		nodes += perftDebug(Board, Hash, depth - 1, -colour);
			//	}
			//	else
			//	{
			nodes += perftCheckHash(Board, Hash, CheckHash, depth - 1, chessMoves.moveRepresentationToNotation(moves[i].move));
			//	}
		}

		Board.revertPiece(moves[i].move, Hash, chessMoves);

		cur_nodes = nodes - prev_nodes;
		std::cout << chessMoves.moveRepresentationToNotation(moves[i].move) << " " << cur_nodes << " " << moves[i].move << std::endl;

	}

	//return perftLeafNodes;
	return nodes;
}


long long Search::perftCheckHash(Gameboard& Board, Hashing& Hash, Hashing& CheckHash, int depth, string parentMove)
{
	long long nodes = 0;
	vector <MoveGen::moveList> moves;
	int n_moves = 0;
	int i = 0;
	int colour = Board.getSideInt();
	
	if (depth == 0)
	{
		return 1;  //return 1 for each leaf node to add to nodes total
	}

	moves = chessMoves.generateMoves(Board, depth);

	for (int i = 0; i < moves.size(); i++)
	{
		Board.movePiece(moves[i].move, Hash, chessMoves);

		if (Hash.getZobristKey() != CheckHash.generatePositionKey(Board))
		{
			std::cout << "=========================================================" << std::endl;
			std::cout << "Perft moves " << i << " Depth " << depth << " Parent move " << parentMove << std::endl;
			std::cout << "Zobrist mismatch on Move " << moves[i].move << " " << chessMoves.moveRepresentationToNotation(moves[i].move) << std::endl;
			std::cout << "Generated Hash Key is       : " << CheckHash.generatePositionKey(Board) << std::endl; //quick and inefficient to see if works
			std::cout << "Board Hash Key is           : " << Hash.getZobristKey() << std::endl;
			std::cout << "=========================================================" << std::endl;
		}
		if (chessMoves.isInCheck(Board, colour) == false)
		//if (Moves.isInCheck(Board) == false)
		{
			nodes += perft(Board, Hash, depth - 1);
		}
		Board.revertPiece(moves[i].move, Hash, chessMoves);
	}

	//return perftLeafNodes;
	return nodes;
}





int Search::probePVTable(LINE line, unsigned long long boardHash)
{
	for (int i = 0; i < line.cmove; i++)
	{
		if (boardHash == line.hash[i])
		{
			return line.argmove[i];
		}
	}
	return 0;
}


int Search::quiescence(Gameboard& Board, Hashing& Hash, int depth, int alpha, int beta, int timePerMove) {
	int colour = Board.getSideInt();
	int score = 0;

	vector <MoveGen::moveList> captureMoves;
	LINE line;

	int standPat = Eval.evaluateBoard(Board) * colour;
	//stand pat stuff. If score is above beta, we can safely return beta
	if (standPat >= beta)
	{
		return beta;
	}

	//delta pruning
	int bigDelta = 905; //value of queen on a good square
	if (standPat < (alpha - bigDelta))
	{
		return alpha;
	}

	if (alpha < standPat) //if score is greater than alpha, increase our alpha to the level of the score without making any captures
	{
		alpha = standPat;
	}
	if (depth == 0)
	{
		return standPat;
	}
	captureMoves.clear();
	captureMoves = chessMoves.generateCaptureMoves(Board, depth);
	if (captureMoves.size() == 0)
	{
		return standPat;
	}
	//sort the moves
	std::sort(captureMoves.begin(), captureMoves.end(), greater()); //Sorting mvvlva captures.


	//until every capture has been examined
	for (int i = 0; i < captureMoves.size(); ++i)
	{
		if (stop == true) { return Defs::StopEngine; }
		Board.movePiece(captureMoves[i].move, Hash, chessMoves);

		if (chessMoves.isInCheck(Board, colour) == false)
		{
			numberOfQNodes++;
			score = -quiescence(Board, Hash, depth - 1, -beta, -alpha, timePerMove);
		}

		Board.revertPiece(captureMoves[i].move, Hash, chessMoves);

		if (score > alpha) { //get the move ordering stats
			if (score >= beta) {
				failQHighFirst++;
			}
			failQHigh++;
		}

		// found a better move
		if (score > alpha)
		{
			if (score >= beta)
			{
				// node (position) fails high
				return beta; //fail hard beta cutoff
			}
			//failQHigh++;
			alpha = score; //alpha acts like max in minimax

		}

	}
	// node (position) fails low
	return alpha;
}


int Search::quiescence(Gameboard& Board, Hashing& Hash,int alpha, int beta) {
	int colour = Board.getSideInt();
	int score = 0;

	vector <MoveGen::moveList> captureMoves;
	LINE line;

	//numberOfQNodes++;
	

	int standPat = Eval.evaluateBoard(Board) * colour;
	//stand pat stuff. If score is above beta, we can safely return beta
	if (standPat >= beta)
	{
		return beta;
	}

	//delta pruning
	int bigDelta = 905; //value of queen on a good square
	if (standPat < (alpha - bigDelta))
	{
		return alpha;
	}

	if (alpha < standPat) //if score is greater than alpha, increase our alpha to the level of the score without making any captures
	{
		alpha = standPat;
	}
	
	captureMoves.clear();
	captureMoves = chessMoves.generateCaptureMoves(Board, 0); //ply parameter is provided for killer heuristic which is not applicable to quiescence
	if (captureMoves.size() == 0)
	{
		return standPat;
	}
	//sort the moves
	std::sort(captureMoves.begin(), captureMoves.end(), greater()); //Sorting mvvlva captures.


	//until every capture has been examined
	for (int i = 0; i < captureMoves.size(); ++i)
	{
		if (stop == true) { return Defs::StopEngine; }
		//Board.printBoard(); //for position prior to q node
		Board.movePiece(captureMoves[i].move, Hash, chessMoves);

		if (chessMoves.isInCheck(Board, colour) == false)
		{
			numberOfQNodes++;
			//std::cout << "=========== Q Nodes =============" << numberOfQNodes << " " << Moves.moveRepresentationToNotation(captureMoves[i].move) << std::endl;
			//Board.printBoard();
			//Board.printPieceList();
			//Moves.moveRepresentationToNotation(captureMoves[i].move);
			score = -quiescence(Board, Hash, -beta, -alpha);
		}

		Board.revertPiece(captureMoves[i].move, Hash, chessMoves);

		if (score > alpha) { //get the move ordering stats
			if (score >= beta) {
				failQHighFirst++;
			}
			failQHigh++;
		}

		if (score >= beta)
		{
			// node (position) fails high
			return beta; //fail hard beta cutoff
		}

		// found a better move
		if (score > alpha)
		{
			alpha = score; //alpha acts like max in minimax
		}
	}
	// node (position) fails low
	return alpha;
}



int Search::negamax(Gameboard& Board, Hashing& Hash, int depth, int pvDepth, int alpha, int beta, LINE* pline, int timePerMove, int mate)
{
	int legal = 0;
	int val = 0;
	int hashFlag = hashfALPHA; // define hash flag
	int hashVal = Hash.ProbeHash(depth, alpha, beta);
	int colour = Board.getSideInt();
	vector <MoveGen::moveList> moves;
	LINE line;

	
	numberOfNodes++;

	if (chessMoves.isInCheck(Board, colour) == true)
	{
		depth++; //increment depth if in check so that we do not miss a mate
		pvDepth++;
	}

	if (hashVal != VALUNKNOWN)
	{
		// if the move has already been searched (hence has a value)
		// we just return the score for this move without searching it

		numberOfHashMatches++;
		val = hashVal;
		return val;
	}
	

	if (depth == 0) //TODO or game over ///Gone to depth of tree. Now evaluate and pass down values
	{
		pline->cmove = 0;
		//val = Eval.evaluateBoard(Board) * colour;
		//val = quiescence(Board, Hash, quiescenceDepth, alpha, beta, timePerMove);
		val = quiescence(Board, Hash, alpha, beta);
		return val;
	}


	//for each child of Board
	moves = chessMoves.generateMoves(Board, depth);
	

	//Score the pv move
	//get the PV for this board position
	int pvMove = probePVTable(previousBestLine, Board.getBoardHash());

	if (pvMove != 0)
	{

		for (int i = 0; i < moves.size(); ++i)
		{
			if (moves[i].move == pvMove)
			{
				moves[i].score = 2000000;
				break;
			}
		}
	}

	//sort the moves
	std::sort(moves.begin(), moves.end(), greater()); //Sorting mvvlva captures and pv sorting. Operator overloaded mvlist

	for (int i = 0; i < moves.size(); ++i)
	{
		if (stop == true)
		{
			val = Defs::StopEngine;
			return val;
		}
		std::chrono::duration <double>(timeElapsed) = std::chrono::steady_clock::now() - start;
		if ((timeElapsed.count() - timePerMove) > 0) {
			val = Defs::StopEngine;
			return val;
		}
		Board.movePiece(moves[i].move, Hash, chessMoves);
		if (chessMoves.isInCheck(Board, colour) == false)
		{
			legal++;
			//numberOfNodes++;
			val = -negamax(Board, Hash, depth - 1, pvDepth, -beta, -alpha, &line, timePerMove, mate - 1);
			
			Board.revertPiece(moves[i].move, Hash, chessMoves);

			if (val > alpha)
			{ //get the move ordering stats
				if (val >= beta)
				{
					failHighFirst++;
				}
				failHigh++;
			}

			// found a better move
			if (val > alpha)
			{
				if (val >= beta)
				{
					// store hash entry with the score equal to beta
					Hash.RecordHash(depth, beta, hashfBETA);

					//load array with killer heuristic
					if (moves[i].quiet == true)
					{
						killers[1][depth] = killers[0][depth];
						killers[0][depth] = moves[i].move;
					}

					// node (position) fails high
					val = beta;
					return beta; //fail hard beta cutoff
				}
				//Search history
				if (moves[i].quiet == true)
				{
					searchHistory[static_cast <int> (Board.getPiece(chessMoves.fromSquare(moves[i].move)))][(chessMoves.toSquare(moves[i].move))]
						+= (depth * depth); //Depth squared to reward moves that have an alpha cutoff nearer to the root

				}
				alpha = val; //alpha acts like max in minimax
				// switch hash flag from storing score for fail-low node
				// to the one storing score for PV node
				hashFlag = hashfEXACT;

				pline->argmove[0] = moves[i].move;
				pline->hash[0] = Board.getBoardHash();
				memcpy(pline->argmove + 1, line.argmove, line.cmove * sizeof(int));
				memcpy(pline->hash + 1, line.hash, line.cmove * sizeof(long long));
				pline->cmove = line.cmove + 1;


				//Best Move
				if (depth == pvDepth)
				{
					bestMove = moves[i].move; //class scoped instance variable set at top-level recursive call
					bestLine.cmove = pline->cmove;
					for (int j = 0; j < pline->cmove; ++j)
					{
						bestLine.argmove[j] = pline->argmove[j];
						bestLine.hash[j] = pline->hash[j];
					}
				}
			}
		}
		else
		{
			Board.revertPiece(moves[i].move, Hash, chessMoves);
		}
	}
		if (legal == 0)
		{
			if (chessMoves.isInCheck(Board, Board.getSideInt()) == false)    // Stalemate.
			//if (Moves.isInCheck(Board) == false)    // Stalemate.
			{
				val = Defs::staleMateScore;
				return val;
			}
			else
			{
				val = -(Defs::mateScore + mate);
				return val;						// Checkmate.
			}
		}
	// store hash entry with the score equal to alpha
	Hash.RecordHash(depth, alpha, hashFlag);
	// node (position) fails low
	return alpha;
}


void Search::printNodes(LINE* pline, int val, Gameboard& Board, int depth)
{
	for (int i = 0; i < pline->cmove; i++)
	{
		Gameboard Board;
		std::cout << i << ":" << Board.indexToNotation(chessMoves.fromSquare(pline->argmove[i])) << "-"
			<< Board.indexToNotation(chessMoves.toSquare(pline->argmove[i])) << " ";
	}
	std::cout << "Value " << val << " Depth " << depth;
	std::cout << " King square White " << Board.indexToNotation(Board.getKingSquare('w')) << " Black " << Board.indexToNotation(Board.getKingSquare('b'));

}

