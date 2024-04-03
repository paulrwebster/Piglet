#pragma once

int devStuff();
int UCI();
int perft();
int perftCheckHash();
void iterate(std::stop_token st, int depth, double moveTime);
double calcMoveTime(int moveTime, int wtime, int btime, int winc, int binc, int movestogo);



