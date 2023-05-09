#pragma once
#include <string>

class Fen
{
private:
	std::string fen;
public:
	void setFen(std::string fen);
	std::string getFen();
	void setOpeningFen();
	bool validateFen(std::string fen);
	std::string reverseActiveColour(std::string fen);
	char getSide(std::string fen);
};