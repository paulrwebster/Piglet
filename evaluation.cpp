#include "evaluation.h"


int Evaluation::evaluateBoard(Gameboard &Board) //this version returns +ve white -ve black
{
    int boardScore = 0;
    
    for (int i=0; i<127; ++i)
    {
        boardScore += evaluateWhitePerspective(Board.getPiece(i), i); //evaluate square i
    }
   
    return boardScore;
}


int Evaluation::evaluateBoard(Gameboard& Board, char player)
{
    int boardScore = 0;
    int squareScore = 0;

    for (int i = 0; i < 127; ++i)
    {
        squareScore = evaluate(Board.getPiece(i), i);
        if (player == Board.getPieceColour((int)Board.getPiece(i))) //if piece is same colour as board side, add score
        {
            boardScore += squareScore;
        }
        else
        {
            boardScore -= squareScore;
        }
        squareScore = 0;
    }
    return boardScore;
}


int Evaluation::evaluateWhitePerspective(Defs::Pieces piece, int square) //this version returns negative for black
{

    int squareScore = 0;
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
        squareScore += whiteKingMiddleGameEval[square];
        //TODO whiteKingEndGameEval
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
        squareScore -= blackKingMiddleGameEval[square];
        //TODO blackKingEndGameEval
        break;
    default:
        break;
    }
    return squareScore;
}


int Evaluation::evaluate(Defs::Pieces piece, int square)
{
    
    int squareScore = 0;
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
        squareScore += whiteKingMiddleGameEval[square];
        //TODO whiteKingEndGameEval
        break;
        //Black pieces 
    case Defs::Pieces::p:
        squareScore += pieceVal[(int)piece];
        squareScore += blackPawnEval[square];
        break;
    case Defs::Pieces::n:
        squareScore += pieceVal[(int)piece];
        squareScore += blackKnightEval[square];
        break;
    case Defs::Pieces::b:
        squareScore += pieceVal[(int)piece];
        squareScore += blackBishopEval[square];
        break;
    case Defs::Pieces::r:
        squareScore += pieceVal[(int)piece];
        squareScore += blackRookEval[square];
        break;
    case Defs::Pieces::q:
        squareScore += pieceVal[(int)piece];
        squareScore += blackQueenEval[square];
        break;
    case Defs::Pieces::k:
        squareScore += pieceVal[(int)piece];
        squareScore += blackKingMiddleGameEval[square];
        //TODO blackKingEndGameEval
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
