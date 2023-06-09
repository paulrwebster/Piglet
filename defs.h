#ifndef _DEFS_H_
#define _DEFS_H_

#define SET_BIT(val, bitIndex) val |= (1 << bitIndex)
#define CLEAR_BIT(val, bitIndex) val &= ~(1 << bitIndex)
#define TOGGLE_BIT(val, bitIndex) val ^= (1 << bitIndex)
#define BIT_IS_SET(val, bitIndex) (val & (1 << bitIndex)) 
#define hashfEXACT   0
#define hashfALPHA   1
#define hashfBETA    2
#define HASHSIZE 0x400000 //int 4,194,304
#define VALUNKNOWN	9999999 //Value Unknown 9999999 greater than alpha beta infinity cutoff of 999999

class Defs
{
public:

	enum class Pieces { empty, P, N, B, R, Q, K, p, n, b, r, q, k, offboard, enpassant };
	enum class Colours { w, b, x };
	static const int NoSquare = 999;
	static const int NoMove = 0;
	static const int MaxSearchDepth = 20;
	static const int MaxSearchTime = 999999999;
	static const int StopEngine = 888888;
	static const int mateScore = 50000;
	static const int staleMateScore = 0;
	static const int infinity = 999999;
	static const int movesToGo = 24;
	static const int moveTimeBuffer = 500; //milliseconds
};

#endif