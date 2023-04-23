#pragma once


#include "Board.h"

enum Value : int {

};
typedef int Depth;

Value search(Board& board, Value alpha, Value beta, Depth depth);