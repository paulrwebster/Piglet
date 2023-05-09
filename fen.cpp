#include <string>
#include "fen.h"
#include <regex>
#include <iostream>
#include <sstream>
#include <vector>


void Fen::setFen(std::string fen){
	Fen::fen = fen;
}
std::string Fen::getFen() {
    return fen;
}
void Fen::setOpeningFen() {
	Fen::fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}
bool Fen::validateFen(std::string fen) {
    std::regex regex_fen("\\s*^(((?:[rnbqkpRNBQKP1-8]+\\/){7})[rnbqkpRNBQKP1-8]+)\\s([b|w])\\s([-|K|Q|k|q]{1,4})\\s(-|[a-h][1-8])\\s(\\d+\\s\\d+)$");
    bool match =
        std::regex_match(fen, regex_fen);
    if (match)
    {   
        std::cout << "Valid FEN: " << fen  << std::endl;
        return true;
    }
    else
    {
        std::cout << "Bad FEN: " << fen  << std::endl;
        return false;
    }
}

std::string Fen::reverseActiveColour(std::string fen)
{
    char side;
    std::string returnFen;
    //Read the parts of the space delimited fen string into a vector    
    std::vector<std::string> fenParts;
    std::string buf;
    std::istringstream iss{ fen };
    while (iss >> buf) {
        fenParts.push_back(buf);
    }

    //Reverse the active colour
    if (fenParts.size() >= 2) {
        side = fenParts.at(1).at(0);
        if (side == 'w') { fenParts.at(1).at(0) = 'b'; }
        if (side == 'b') { fenParts.at(1).at(0) = 'w'; }
    }

    std::string rebuildFen;
    //write the parts back to a string
    for (std::string s : fenParts) //range based for loop
    {
        rebuildFen += s;
        rebuildFen += " ";
    }
    returnFen = rebuildFen.substr(0, rebuildFen.size() - 1); //Remove the trailing space
    //std::cout << "fen is          : |" << fen << "|" << std::endl;
    //std::cout << "modified fen is : |" << returnFen << "|" << std::endl;
    

    return returnFen;
}

char Fen::getSide(std::string fen)
{
    char side = 'w';
    //Read the parts of the space delimited fen string into a vector    
    std::vector<std::string> fenParts;
    std::string buf;
    std::istringstream iss{ fen };
    while (iss >> buf) {
        fenParts.push_back(buf);
    }

    if (fenParts.size() >= 2) {
        side = fenParts.at(1).at(0);
    }
    return side;
}
