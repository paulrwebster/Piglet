#include <iostream>
#include <string>
#include <iomanip>   //setw
#include <sstream>
#include <functional> //std::ref
#include <future>	//std::promise, std::future	
#include <thread>	//std::thread
#include "gameboard.h"
#include "piglet.h"
#include "fen.h"
#include <bitset>
#include <string>
#include <chrono> //for clock
#include "moveGen.h"
#include "evaluation.h"
#include "search.h"
#include "hashing.h"


Gameboard* Board = new Gameboard();

Fen* BoardFen = new Fen();
MoveGen* Moves = new MoveGen();
Evaluation* EvalMain = new Evaluation();
Search* Minimax = new Search();
Hashing* Hash = new Hashing();
Hashing* CheckHash = new Hashing();  //for use in perftCheckHash function

int killers[2][Defs::MaxSearchDepth] = { 0 };
int searchHistory[15][128]; //[number of pieces][number of squares]
chrono::steady_clock::time_point start;
bool debug = false;
bool ponderOption = false;

vector<string> guiMoves; //holds the moves list sent by the gui as part of startpos moves command
vector<string> goParts;  //holds the parsed parts of the go string

int main()
{
	UCI();
	//devStuff();
	//perft();
	//perftCheckHash();


	delete Board;
	delete BoardFen;
	delete Moves;
	delete EvalMain;
	delete Minimax;
	delete Hash;
	delete CheckHash;
	std::cout << "Clean up complete " << std::endl;
	return 0;
}

int UCI()
{
	//setoption name Ponder value false

	int depth = Defs::MaxSearchDepth - 1; //will be overriden by go depth command from gui
	int moveTime = 0;
	int wtime = 0;
	int btime = 0;
	int winc = 0;
	int binc = 0;
	int movestogo = Defs::movesToGo;
	int movestimebuffer = Defs::moveTimeBuffer;
	string Line; //to read the command given by the GUI
	string startpos; //hold the opening position fen
	std::stop_source ss;
	std::stop_token st = ss.get_token(); //stop token to stop iterate jthread
	std::cout.setf(ios::unitbuf);// Make sure that the outputs are sent straight away to the GUI
	std::cout << fixed << setprecision(0);

	while (getline(cin, Line)) {

		if (Line == "uci") {
			std::cout << "id name Piglet 1.31" << std::endl;
			std::cout << "id author Paul Webster" << std::endl;
			std::cout << "option name Ponder type check" << std::endl;
			std::cout << "uciok" << std::endl;

		}



		else if (Line == "quit") {
			//break from loop and let main() clean up and terminate
			std::cout << "quit received " << std::endl;
			//Minimax->stopEngine();
			//Minimax->stopPondering();
			//Minimax->setPonderhit(false);

			try {
				//	exit(0);
				ss.request_stop();
				return 0;
			}
			catch (std::exception& e) {
				std::cout << "exception " << e.what() << std::endl;
			}

		}

		else if (Line == "debug on")
		{
			debug = true;
		}
		else if (Line == "debug off")
		{
			debug = false;
		}
		else if (Line == "isready") {
			std::cout << "readyok" << std::endl;
		}
		else if (Line.substr(0, 10) == "ucinewgame")
		{
			//Initialise
			Board->initSquare(); //Initialise squares
			Hash->generateHashKeys();
			Moves->setAttacksArray();
			Minimax->initMoveGenMvvLva();
			//Clear
			Minimax->clearBestLine();
			Minimax->clearBestMove();
			Moves->initialiseMoves();
			Minimax->clearPreviousBestLine();
			Minimax->clearPreviousBestMove();
			Moves->initialiseMoves();
			Hash->clearHashTable();
			guiMoves.clear(); //Clear list of moves sent by gui as startpos moves
			startpos.clear();
			Minimax->resetNumberOfNodes();
			Minimax->resetNumberOfHashMatches();
			Minimax->stopPondering();
			Minimax->setPonderhit(false);
		}

		else if (Line == "setoption name Ponder value false")
		{
			ponderOption = false;
			Minimax->stopPondering();
		}

		else if (Line == "setoption name Ponder value true")
		{
			ponderOption = true;
			Minimax->startPondering();
		}

		else if (Line.substr(0, 8) == "position")
		{
			if (Line == "position startpos") {
				//set the startpos fen
				startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
			}

			else if (Line.substr(0, 23) == "position startpos moves") { //stream the moves into vector guiMoves
				if (startpos == "") //white has moved first so no "position startpos" or  "startpos fen" supplied. 
				{
					startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
				}
				guiMoves.clear();
				string buf;
				istringstream iss{ Line.substr(24,string::npos) };
				while (iss >> buf) {
					guiMoves.push_back(buf);
				}
			}

			//could be just a position fen or position fen 4k3/8/8/8/1n6/8/8/4K2R w K - 0 3 moves e1g1 e8d8
			else if (Line.substr(0, 12) == "position fen")
			{
				size_t found = Line.find("moves");
				if (found == string::npos) //moves not found in string
				{
					startpos = Line.substr(13, string::npos);
				}
				else
				{
					startpos = Line.substr(13, found - 1); //process the fen
					//process the guiMoves
					guiMoves.clear();
					string buf;
					istringstream iss{ Line.substr(found + 5,string::npos) };  // found + 5 is 19?
					while (iss >> buf)
					{
						guiMoves.push_back(buf);
					}
				}

			}

			Board->initSquare(); //Initialise squares
			Board->initBoard(); //Clear the board     
			Moves->initialiseMoves();
			Board->parseFen(startpos); //parse the starting position fen
			if (debug == true)
			{
				std::cout << "after parseFen " << Board->getSideInt() << " " << Board->getSide() << std::endl;
			}
			Board->updatePieceList();
			Board->setKingSquares();

			for (int m = 0; m < guiMoves.size(); ++m) //Make the moves in the guiMoves vector
			{
				int move = Moves->notationToMoveRepresentation(guiMoves[m], *Board);
				Board->movePiece(move, *Hash, *Moves);
			}
			if (debug == true)
			{
				int boardScore = EvalMain->evaluateBoard(*Board); //evaluate the board
				Board->printBoard();
				Board->printPieceList();
			}
		}
		else if (Line == "stop") {
			std::cout << "stop received " << std::endl;
			Minimax->stopEngine();
			Minimax->stopPondering();
			Minimax->setPonderhit(false);
		}

		else if (Line == "print piecelist")
		{
			Board->printPieceList();
		}
		else if (Line == "print board")
		{
			Board->printBoard();
		}

		else if (Line == "ponderhit")
		{
			//if (debug == true) { std::cout << "Loop has received ponderhit " << std::endl; }
			std::cout << "Ponderhit received" << std::endl;
			Minimax->stopPondering();
			//reset the clock
			start = std::chrono::steady_clock::now();
			Minimax->setPonderhit(true);
		}

		else if (Line.substr(0, 3) == "go ")
		{
			// Received a command like: "go wtime 300000 btime 300000 winc 0 binc 0"


			//parse the string
			goParts.clear();
			string buffer;
			istringstream ss{ Line.substr(0,string::npos) };
			while (ss >> buffer) {
				goParts.push_back(buffer);
			}

			if (goParts[1] == "perft")
			{
				depth = stoi(goParts[2]);
				long long perftLeafNodes = Minimax->divide(*Board, *Hash, depth);
				std::cout << "Perft returns " << perftLeafNodes << std::endl;
			}
			else if (goParts[1] == "ponder")
			{
				Minimax->startPondering();

				if (goParts[2] == "movetime")
				{
					depth = Defs::MaxSearchDepth;
					moveTime = static_cast <int> (stoi(goParts[3]) / 1000);
				}

				if (goParts[2] == "depth")
				{
					depth = stoi(goParts[3]);
					moveTime = Defs::MaxSearchTime;
				}

				if (goParts[2] == "infinite")
				{
					depth = Defs::MaxSearchDepth;
					moveTime = Defs::MaxSearchTime;
				}
			}
			else
			{
				for (int i = 0; i < goParts.size(); i++)
				{
					int j = i + 1;
					if (goParts[i] == "movetime")
					{
						depth = Defs::MaxSearchDepth;
						moveTime = static_cast <int> (stoi(goParts[j]) / 1000);
					}
					if (goParts[i] == "depth")
					{
						depth = stoi(goParts[j]);
						moveTime = Defs::MaxSearchTime;
					}
					if (goParts[i] == "infinite")
					{
						Minimax->setInfinite(true);
						moveTime = Defs::MaxSearchTime;
						depth = Defs::MaxSearchDepth;
					}
					if (goParts[i] == "wtime") wtime = stoi(goParts[j]);
					if (goParts[i] == "btime") btime = stoi(goParts[j]);
					if (goParts[i] == "winc") winc = stoi(goParts[j]);
					if (goParts[i] == "binc") binc = stoi(goParts[j]);
					if (goParts[i] == "movestogo") movestogo = stoi(goParts[j]);
				}

			}
			int timeAllowed = calcMoveTime(moveTime, wtime, btime, winc, binc, movestogo);
			//if (debug)
			//{
				std::cout << "Time allowed " << timeAllowed << std::endl;
			//}
			//std::async(std::launch::async, iterate, depth, moveTime, debug);
			
			
			std::jthread iteration(iterate, st, depth, timeAllowed);

			//std::thread iteration(iterate, depth, moveTime);
			iteration.detach();
			//iterate(depth, timeAllowed);
		}

	}
	return 0;
}

int calcMoveTime(int moveTime, int wtime, int btime, int winc, int binc, int movestogo)
{
	int timeAllowed = 0;
	if (moveTime > 0) timeAllowed = moveTime;
	char side = Board->getSide();
	if (side == 'w' && (wtime + winc) > 0) timeAllowed = (wtime + winc) / movestogo / 1000;
	if (side == 'b' && (btime + binc) > 0) timeAllowed = (btime + binc) / movestogo / 1000;
	return timeAllowed;
}

void iterate(std::stop_token st, int depth, int moveTime)
{
	int bestMove = Defs::NoMove;
	int ponderMove = Defs::NoMove;
	// clear the best moves and best lines


	Search::LINE bestLine;
	//get starting timepoint
	start = std::chrono::steady_clock::now();
	Minimax->resetNumberOfNodes();
	int retVal = 0;
	int retValPonder = 0;
	Search::LINE linePtr;
	linePtr.cmove = 0; //ToDo: do we need this as initialised in loop
	for (int i = 0; i < Defs::MaxSearchDepth; ++i)
	{
		linePtr.argmove[i] = 0;
	}

	//Hash->clearHashTable(); //only flush hash table on ucinewgame
	Minimax->clearBestLine();
	Minimax->clearBestMove();
	Minimax->clearPreviousBestLine();
	Minimax->clearPreviousBestMove();

	Moves->initialiseMoves();//this clears the vector of moves in MoveGen, plus set attacks array



	//iterative deepening stuff
	int search_depth = depth;
	int ponder_depth = depth;
	Minimax->startEngine();
	for (int current_depth = 1; current_depth <= search_depth; current_depth++)
	{
		if (st.stop_requested()) {
			std::cout << "stop token received by thread " << std::endl;
			return;
		}

		Minimax->clearSearch();  //yes
		Minimax->resetNumberOfNodes(); //yes
		Minimax->resetNumberOfHashMatches();
		linePtr.cmove = 0;
		for (int i = 0; i < Defs::MaxSearchDepth; ++i)
		{
			linePtr.argmove[i] = 0;
		}

		//&Search::negamax is member function pointer, Minimax is a pointer to the object to call the function for
		//auto future1 = std::async(std::launch::async, &Search::negamax, Minimax, std::ref(*Board), std::ref(*Hash), current_depth, current_depth, ponder_depth, -Defs::infinity, Defs::infinity, &linePtr, moveTime, Defs::MaxSearchDepth);
		//retVal = future1.get();
		retVal = Minimax->negamax(st ,*Board, *Hash, current_depth, current_depth, ponder_depth,
			-Defs::infinity, Defs::infinity, &linePtr, moveTime, Defs::MaxSearchDepth);

		double mateDepth = 0;
		if (retVal >= (Defs::mateScore) && retVal <= (Defs::mateScore + Defs::MaxSearchDepth))
		{
			mateDepth = ceil((Defs::MaxSearchDepth - (retVal - Defs::mateScore)) / 2); //UCI requires mate in y moves, not plies
		}
		if (retVal <= (-Defs::mateScore) && retVal >= (-Defs::mateScore - Defs::MaxSearchDepth))
		{
			mateDepth = -ceil((Defs::MaxSearchDepth - (abs(retVal) - Defs::mateScore)) / 2); //If the engine is getting mated use negative values for moves
		}

		if (retVal == Defs::StopEngine)
		{
			if (debug == true)
			{
				std::cout << "stop command received by engine " << std::endl;
			}
			//bestMove = Minimax->getPreviousBestMove();
			bestLine = Minimax->getPreviousBestLine();
			if (bestLine.cmove >= 1) { bestMove = bestLine.argmove[0]; }
			if (bestLine.cmove >= 2) { ponderMove = bestLine.argmove[1]; }
			search_depth = current_depth; //stop loop falling through to maxdepth if timed or search_depth if by depth
		}
		else
		{

			//bestMove = Minimax->getBestMoveFound();
			bestLine = Minimax->getBestLine();
			if (bestLine.cmove >= 1) { bestMove = bestLine.argmove[0]; }
			if (bestLine.cmove >= 2) { ponderMove = bestLine.argmove[1]; };
			Minimax->setPreviousBestLine(bestLine); //copy best line into previous best line for pv sorting
			Minimax->setPreviousBestMove(bestMove);
			if (debug == true)
			{
				//std::cout << "Depth " << current_depth << " Best Move " << Moves->moveRepresentationToNotation(bestMove) << " Score " << retVal << std::endl;
			}
			int failHigh = Minimax->getFailHigh();
			int failHighFirst = Minimax->getFailHighFirst();
			if (debug == true)
			{
				if (failHigh > 0 && failHighFirst > 0) {
					float ordering = (static_cast<float> (failHighFirst) / static_cast<float> (failHigh)) * 100;
					std::cout << "Depth: " << current_depth << " Ordering: " << ordering << " % " << std::endl;
				}
			}
			int QFailHigh = Minimax->getFailQHigh();
			int QFailHighFirst = Minimax->getFailQHighFirst();
			if (debug == true)
			{
				if (QFailHigh > 0 && QFailHighFirst > 0) {
					float ordering = (static_cast<float> (QFailHighFirst) / static_cast<float> (QFailHigh)) * 100;
					std::cout << "Depth: " << current_depth << " Quiescence Ordering: " << ordering << " % " << std::endl;
				}
				std::cout << "--------PV--------" << std::endl;
				for (int i = 0; i < bestLine.cmove; ++i) {
					std::cout << (Moves->moveRepresentationToNotation(bestLine.argmove[i])) << " ";
				}
				std::cout << std::endl;
			}
		}
		chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
		chrono::duration<double> time_span = std::chrono::duration_cast<chrono::duration<double>>(stop - start);
		size_t numberOfNodes = Minimax->getNumberOfNodes();
		double nps = (int)numberOfNodes / (double)time_span.count();
		string pv;
		for (int i = 0; i < Minimax->getBestLine().cmove; ++i) {
			pv += (Moves->moveRepresentationToNotation(Minimax->getBestLine().argmove[i]));
			pv += " ";
		}

		if (mateDepth == 0)
		{
			std::cout << "info depth " << current_depth << " score cp " << retVal << " time " << (int)time_span.count()
				<< " nodes " << numberOfNodes << " nps " << nps << " pv " << pv << std::endl;
		}
		else
		{
			std::cout << "info depth " << current_depth << " score mate " << static_cast <int> (mateDepth) << " time " << (int)time_span.count()
				<< " nodes " << numberOfNodes << " nps " << nps << " pv " << pv << std::endl;
		}
		if (debug == true)
		{
			std::cout << "Number of hash matches " << Minimax->getNumberOfHashMatches()
				<< " Number of quiescence nodes " << Minimax->getNumberOfQNodes() << std::endl;
		}
		if (Minimax->isPonderhit() == true)
		{
			//turn it into a normal search
			if (debug == true) { std::cout << "setPonderhit(false) called " << std::endl; }
			Minimax->setPonderhit(false);
			if (debug == true) { std::cout << "stopPondering called " << std::endl; }
			//Minimax->stopPondering();
			search_depth = depth;
			//if (current_depth > depth)
			//{
			//	Minimax->stopEngine();
			//	break;
			//}
		}

		//extend search if pondering
		if (Minimax->isPondering() == true && search_depth >= depth) { search_depth++; }
	}

		bestMove = Minimax->getBestMoveFound();
		string bestMoveNotation = Moves->moveRepresentationToNotation(bestMove);
		string ponderMoveNotation = Moves->moveRepresentationToNotation(ponderMove);
		Board->movePiece(bestMove, *Hash, *Moves);
		if (debug == true)
		{
			//std::cout << "string bestMove is " << bestMoveNotation << std::endl;
			//Board->printBoard();
			//Board->printPieceList();
			//Moves->printMoveRepresentation(bestMove);
		}
		if (ponderOption == true && ponderMove != Defs::NoMove)
		{
			std::cout << std::endl << "bestmove " << bestMoveNotation << " ponder " << ponderMoveNotation << std::endl;
		}
		else
		{
			std::cout << std::endl << "bestmove " << bestMoveNotation << std::endl;
		}
	
}


int devStuff()
{
	int depth = 3;
	bool iterativeDeepening = false;
	//int moveTime = 6;
	int moveTime = Defs::MaxSearchTime;
	Board->initSquare(); //Initialise squares
	Board->initBoard(); //Clear the board    
	int cFlag = Board->getCastleFlag();
	Hash->clearHashTable();
	std::cout << "Generate position keys " << std::endl;
	Hash->generateHashKeys();
	Hash->generatePositionKey(*Board);
	std::stop_source ss;
	std::stop_token st = ss.get_token();
	//set up the board
	Board->parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); //opening board
	//Board->parseFen("rnbqkbnr/pppppppp/8/8/8/5N2/PPPPPPPP/RNBQKB1R b KQkq - 0 1"); //opening board plus knight move
	//Board->parseFen("rnb1k2r/pp2qppp/3p1n2/2pp2B1/1bP5/2N1P3/PP2NPP/R2QKB1R w KQkq - 0 1"); //Bluefever video 50 Middle Game Position. Demos quiescence at depth 3
	//Board->parseFen("r1bqkb1r/ppp1pppp/3p4/1B6/3QP3/2N5/PPP2PPP/R1B1K2R b KQkq - 0 1");//test check depth increase
	//Board->parseFen("4kq2/8/8/B1b5/8/1bB5/8/4KQ2 b - - 0 1");//test check end game
	//Board->parseFen("4k3/Pppp1ppp/8/8/8/8/Pp1P2PP/5K2 w - - 0 1"); //test promotion
	//Board->parseFen("4kqr1/8/8/8/8/8/8/2K5 b - - 0 1");//test mate
	//position fen 4k3/Pppp1ppp/8/8/8/8/Pp1P2PP/5K2 w - -0 1 moves a7a8q e8e7 f1f2 b2b1b                                                      engine.py : 1023
	//go wtime 55189 btime 67680 winc 8000 binc 8000 movetime 12000

	Board->printBoard();
	Board->updatePieceList();
	Board->setKingSquares();
	Board->printMaterial();
	Board->printPieceList();
	std::cout << endl;

	//Move generation
	int retVal = 0;  //returned value
	int retMove = 0; //returned move

	bool maximising = true;
	Minimax->initMoveGenMvvLva();


	Search::LINE linePtr;

	//get starting timepoint
	start = std::chrono::steady_clock::now();

	Minimax->resetNumberOfNodes();
	Minimax->resetNumberOfHashMatches();
	Minimax->resetNumberOfQHashMatches();
	Minimax->clearSearch();


	int evaluation = EvalMain->evaluateBoard(*Board);
	std::cout << "Initial board evaluation from white perspective " << evaluation << std::endl;

	int bestMove = 0;
	int mateDepth = 0;
	Search::LINE bestLine;
	//Test end game
	//if (EvalMain->isEndGame(*Board) == true)
	//{
	//	std::cout << "End Game " << std::endl;
	//}
	//else
	//{
	//	std::cout << "Middle Game " << std::endl;
	//}


	if (iterativeDeepening == false)
	{
		//======================No iterative deepening stuff==========================

		string parentMove = "";
		retVal = Minimax->negamax( st,*Board, *Hash, depth, depth, depth, -Defs::infinity, Defs::infinity, &linePtr, moveTime, Defs::MaxSearchDepth);
		std::cout << "from main.cpp negamax returns retval " << retVal << " bestmove " << Minimax->getBestMoveFound() << " " << Moves->moveRepresentationToNotation(Minimax->getBestMoveFound()) << std::endl;

		mateDepth = 0;
		if (retVal >= (Defs::mateScore) && retVal <= Defs::mateScore + Defs::MaxSearchDepth)
		{
			mateDepth = Defs::MaxSearchDepth - (retVal - Defs::mateScore);
		}
		bestMove = Minimax->getBestMoveFound();
		bestLine = Minimax->getBestLine();
		std::cout << "Number of nodes " << Minimax->getNumberOfNodes() << " Quiescence nodes " << Minimax->getNumberOfQNodes() << std::endl;
		std::cout << "Number of hash matches " << Minimax->getNumberOfHashMatches() << std::endl;
		std::cout << "King square White " << Board->indexToNotation(Board->getKingSquare('w')) << " Black " << Board->indexToNotation(Board->getKingSquare('b')) << std::endl;
		//======================End of no iterative deepening stuff===================
	}
	else
	{
		//======================Iterative deepening stuff=============================

		bestMove = 0;
		mateDepth = 0;
		for (int current_depth = 1; current_depth <= depth; current_depth++)
		{
			Minimax->resetNumberOfNodes();
			Minimax->clearSearch();
			mateDepth = 0;
			linePtr.cmove = 0;
			for (int i = 0; i < Defs::MaxSearchDepth; ++i)
			{
				linePtr.argmove[i] = 0;
			}
			retVal = Minimax->negamax(st, *Board, *Hash, current_depth, current_depth, current_depth, -Defs::infinity, Defs::infinity, &linePtr, moveTime, Defs::MaxSearchDepth);

			std::cout << "from main.cpp negamax returns retval " << retVal << " bestmove " << Minimax->getBestMoveFound() << " " << Moves->moveRepresentationToNotation(Minimax->getBestMoveFound()) << std::endl;

			int mateDepth = 0;
			if (retVal >= (Defs::mateScore) && retVal <= Defs::mateScore + Defs::MaxSearchDepth)
			{
				mateDepth = Defs::MaxSearchDepth - (retVal - Defs::mateScore);
			}
			if (retVal == Defs::StopEngine)
			{
				std::cout << "stop command received by engine " << std::endl;
				bestMove = Minimax->getPreviousBestMove();
				break;
			}
			else
			{
				bestMove = Minimax->getBestMoveFound();
				bestLine = Minimax->getBestLine();
				Minimax->setPreviousBestLine(bestLine); //copy best line into previous nest tline for pv sorting
				Minimax->setPreviousBestMove(bestMove);

				std::cout << "info Depth " << current_depth << " Best Move " << Moves->moveRepresentationToNotation(bestMove) << " Score " << retVal << std::endl;
				std::cout << "Number of nodes " << Minimax->getNumberOfNodes() << " Quiescence nodes " << Minimax->getNumberOfQNodes() << " "
					<< float(((float)Minimax->getNumberOfQNodes() / (float)Minimax->getNumberOfNodes()) * 100) << "%" << std::endl;
				//move ordering stats
				int failHigh = Minimax->getFailHigh();
				int failHighFirst = Minimax->getFailHighFirst();
				int failQHigh = Minimax->getFailQHigh();
				int failQHighFirst = Minimax->getFailQHighFirst();


				if (failHigh > 0 && failHighFirst > 0) {
					float ordering = (static_cast<float> (failHighFirst) / static_cast<float> (failHigh)) * 100;
					float orderingQ = (static_cast<float> (failQHighFirst) / static_cast<float> (failQHigh)) * 100;
					std::cout << "Depth: " << current_depth << " Ordering: " << ordering << " % " <<
						" Quiescence Ordering: " << orderingQ << " % " << std::endl;
				}
				std::cout << "--------PV--------" << std::endl;
				for (int i = 0; i < bestLine.cmove; ++i) {
					std::cout << (Moves->moveRepresentationToNotation(bestLine.argmove[i])) << " ";
				}

				std::cout << std::endl << std::endl;
			}

		}

		//======================End of iterative deepening stuff============================
	}

	//======================rest of the stuff========================================
	string bestMoveNotation = Moves->moveRepresentationToNotation(bestMove);
	std::cout << "string bestMove is " << bestMove << endl;
	std::cout << "bestMove is " << bestMoveNotation << endl;
	std::cout << "pv ";
	for (int i = 0; i < bestLine.cmove; ++i) {
		std::cout << (Moves->moveRepresentationToNotation(bestLine.argmove[i])) << " ";
	}
	std::cout << endl;
	Board->movePiece(bestMove, *Hash, *Moves);
	Board->printBoard();

	Board->printMaterial();
	Board->printPieceList();
	chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
	chrono::duration<double> time_span = std::chrono::duration_cast<chrono::duration<double>>(stop - start);
	size_t numberOfNodes = Minimax->getNumberOfNodes();
	double nps = (int)numberOfNodes / (double)time_span.count();
	if (mateDepth == 0)
	{
		std::cout << "info depth " << depth << " score cp " << retVal << " time " << (int)time_span.count()
			<< " nodes " << numberOfNodes << " nps " << nps << " quiescence nodes " << Minimax->getNumberOfQNodes() << std::endl;
	}
	else
	{
		std::cout << "info depth " << depth << " score mate " << static_cast <int> (mateDepth) << " score cp " << retVal <<
			" time " << (int)time_span.count()
			<< " nodes " << numberOfNodes << " nps " << nps << " quiescence nodes " << Minimax->getNumberOfQNodes() << std::endl;
	}
	std::cout << "Number of hash matches " << Minimax->getNumberOfHashMatches() << std::endl;
	//======================End of the rest of the stuff========================================

	return 0;
}

int perft()
{
	//Hash stuff as need by movePieceHash function
	Hash->clearHashTable();
	Hash->generateHashKeys();

	Board->initSquare(); //Initialise squares
	Board->initBoard(); //Clear the board    

	int cFlag = Board->getCastleFlag();

	Board->parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); //perft position 1
	//Board->parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"); //perft position 2
	//Board->parseFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"); //perft position 3
	//Board->parseFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");   // perft position 4 - promotions and castling
	//Board->parseFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"); //perft position 5
	//Board->parseFen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -0 10"); //perft position 6
	//Board->parseFen("rnb1k2r/pp2qppp/3p1n2/2pp2B1/1bP5/2N1P3/PP2NPP/R2QKB1R w KQkq - 0 1"); //Bluefever video 50 Middle Game Position. Demos quiescence at depth 3
	//Board->parseFen("4k3/R7/8/8/8/8/8/4K2R w - -0 1"); //test checkmate

	std::cout << endl;
	Board->printBoard();
	Board->updatePieceList();
	Board->setKingSquares();
	Board->printMaterial();
	Board->printPieceList();
	std::cout << endl;
	//Moves->setAttacksArray();
	//Move generation
	int depth = 1;
	bool maximising = true;
	int infinity = 999999;
	Search::LINE linePtr;

	//================Inititialise Hash stuff=======================================

	//Hash stuff as need by movePieceHash function
	Hash->clearHashTable();
	Hash->generateHashKeys();
	Hash->generatePositionKey(*Board);



	Hash->clearHashTable();
	Minimax->resetNumberOfNodes();
	Minimax->resetNumberOfHashMatches();
	Minimax->resetNumberOfQHashMatches();
	//std::cout << "Generate position keys " << std::endl;
	Hash->generateHashKeys();





	char player = 'w';


	//int move = 246404; //e1f1
	//player = 'b';
	//Board->movePieceHash(move, player, *Hash, *Moves);
	//Board->movePieceHash(move,  *Hash, *Moves);



	//move = 12288; //a1a7
	//player = 'w';
	//Board->movePieceHash( move, player, *Hash, *Moves);


	//move = 14963; //d8e8
	//player = 'b';
	//Board->movePieceHash(move, player, *Hash, *Moves);

	//move = 15239; //
	//player = 'w';
	//Board->movePieceHash(move, player, *Hash, *Moves);

	//move = 2337; //
	//player = 'w';
	//Board->movePieceHash(move, player, *Hash, *Moves);

	//move = 533; //
	//player = 'b';
	//Board->movePieceHash(move, player, *Hash, *Moves);


	std::cout << "After any manual move " << std::endl;
	std::cout << "====================== " << std::endl << std::endl;
	Board->printBoard();
	Board->printPieceList();
	std::cout << std::endl;

	//Board->printPieceList();
	//cout << (int)Board->getPiece(32) << endl;
	//std::cout << std::endl;
	long long perftLeafNodes = 0;
	//perftLeafNodes = Minimax->>perft(*Board,3, Board->getSideInt());
	std::cout << "Side " << Board->getSideInt() << std::endl;
	//if (player == 'w')
	//{
	perftLeafNodes = Minimax->divide(*Board, *Hash, depth);
	//}
	//else
	//{
	//    perftLeafNodes = Minimax->divide(*Board, *Hash, depth, -Board->getSideInt());

   //}
	std::cout << "Perft returns " << perftLeafNodes << std::endl;

	return 0;
}


int perftCheckHash()
{
	//Hash stuff as need by movePieceHash function
	Hash->clearHashTable();
	Hash->generateHashKeys();
	CheckHash->clearHashTable();
	CheckHash->generateHashKeys();

	//Hash->generatePositionKey(*Board);

	//Gameboard* Board = Board->getInstance();
	Board->initSquare(); //Initialise squares
	Board->initBoard(); //Clear the board    

	int cFlag = Board->getCastleFlag();

	Board->parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); //standard opening
	//Board->parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"); //perft position 2
	//Board->parseFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"); //perft position 3
	//Board->parseFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");   // perft position 4 - promotions and castling
	//Board->parseFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"); //perft position 5
	//Board->parseFen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -0 10"); //perft position 6


	std::cout << endl;
	Board->printBoard();
	Board->updatePieceList();
	Board->setKingSquares();
	Board->printMaterial();
	Board->printPieceList();
	std::cout << endl;
	int depth = 4;
	bool maximising = true;
	int infinity = 999999;
	Search::LINE linePtr;

	//================Inititialise Hash stuff=======================================

	//Hash stuff as need by movePieceHash function
	unsigned long long h = Hash->generatePositionKey(*Board);
	unsigned long long c = CheckHash->generatePositionKey(*Board);
	std::cout << "Opening Position Key Hash      :" << h << std::endl;
	std::cout << "Opening Position Key CheckHash :" << c << std::endl;
	if (c == h) { std::cout << "Opening Zobrist Keys Match" << std::endl; }
	else
	{
		std::cout << "Opening Zobrist Keys Mismatch" << std::endl;
	}

	Minimax->resetNumberOfNodes();
	Minimax->resetNumberOfHashMatches();
	Minimax->resetNumberOfQHashMatches();


	//MOVES FOR POSN3 DEPTH2
	//int move = 943199031; //h4g3
	//player = 'w';
	//Board->movePieceHash(move, player, *Hash, *Moves);


	//std::cout << "Generated Hash Key after initial moves is  : " << CheckHash->generatePositionKey(*Board) << std::endl; //quick and inefficient to see if works
	//std::cout << "Board Hash Key after initial moves is      : " << Hash->getZobristKey() << std::endl;



	//moves for perft4 depth 3
	// int move = 540594592; //a3d3
	//player = 'w';
	//Board->movePieceHash(move, player, *Hash, *Moves);

	//std::cout << "Generated Hash Key after initial moves is  : " << CheckHash->generatePositionKey(*Board) << std::endl; //quick and inefficient to see if works
	//std::cout << "Board Hash Key after initial moves is      : " << Hash->getZobristKey() << std::endl;

	long long perftLeafNodes = Minimax->divideCheckHash(*Board, *Hash, *CheckHash, depth);
	std::cout << "Perft returns " << perftLeafNodes << std::endl;

	return 0;
}


