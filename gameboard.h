#ifndef _GAME_BOARD_H_
#define _GAME_BOARD_H_

#include <string>
#include <vector>
#include "defs.h"

class Hashing; //forward declaration
class MoveGen;

using namespace std;

class Gameboard
{
    private:
    //Gameboard(); //private constructor for singleton

    string asciiColours[2]{ "White", "Black" };
    
    int pieceColourInt[15]{ 2, 0, 0, 0, 0, 0, 0, 1, 1 ,1, 1, 1, 1, 2, 2 }; //0 w 1 b 2 x see defs::colours
    char pieceColour[15]{ '-', 'w', 'w', 'w', 'w', 'w', 'w', 'b', 'b', 'b', 'b', 'b', 'b', 'x', 'x' };
    int pieceVal[15]{ 0,100,320,330,500,900,20000,100,320,330,500,900,20000,0, 0 };
    Defs::Pieces pieceBoard[128];
    char file[8]{ 'a','b','c','d','e','f','g','h' };
    const char rank[8]{ '1','2','3','4','5','6','7','8' };
    int square[128];
    char side = 'w';   //w or b
    int  castleFlag = 0; // 0001, 0010, 0100, 1000 Will use bitwise &
                     //k = 1, q = 2, K = 4, Q = 8   
    int halfMove = 0;
    int fullMove = 0;
    int whiteKingSquare;
    int blackKingSquare;
    unsigned long long hash = 0;

	public: 
    //static Gameboard* instance;
    //static Gameboard* getInstance();
    Gameboard();
    ~Gameboard();
    void setBoardHash(unsigned long long boardHash);
    unsigned long long getBoardHash();
    int epRemovePieceList();
    void epAddPieceList(int square);
    void initSquare();
    void initBoard();
    void printBoard();
    char getSide();
    int getSideInt();
    void setSide(char);
    char getPieceColour(int piece);
    int getPieceColourInt(int piece);
    char getSquareColour(int square);
    int getCastleFlag();
    int setCastleFlag(int flag);
    int clearCastleFlag();
    void setKingSquares();
    int getKingSquare(char side);
    int clearEnpassantFlag(char side);
    char asciiPieces[15]{ '-', 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', 'x', 'e' }; // '-' empty, 'x' offboard, 'e enpassant'

    //piece list stuff
    int material[3] = { 0 }; //WHITE w, BLACK b, x material of pieces
    int pieceCount[15] = {0}; // The count of each piece indexed by Defs::Pieces
    int pieceList[16 * 10]; //allows up to 10 of each piece (allowing for promotions) i.e piece 15 x 10 + 10 = 160
    int PieceIndex(int piece, int pieceCount);
    void printMaterial();
    void printPieceList();
    void updatePieceList();
    int getPieceColour(Defs::Pieces piece);
    int getPieceSquare(int piece, int count);

    unsigned int notationToIndex(char file, char rank);
    unsigned int notationToIndex(string notation);
    unsigned int notationToIndex(int file, int rank);
    unsigned int rankAlgabraicToInt(char rank);
    unsigned int fileAlgabraicToInt(char file);
    string indexToNotation(int index);
    int indexToFile(int index);
    int parseFen(string fen);
    bool onBoard(int index);
    char pieceToAscii(Defs::Pieces p);
    int asciiPieceToInt(char piece);
    Defs::Pieces getPiece(int square);
    void setPiece(int square, Defs::Pieces piece);
    //void movePiece(string notation);
    void movePiece(int& move, Hashing& Hash, MoveGen& Moves);
    void revertPiece(int& move, Hashing& Hash, MoveGen& Moves);
    char swapSide();
};

#endif;