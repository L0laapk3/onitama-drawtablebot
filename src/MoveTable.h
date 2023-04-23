#pragma once

#include <array>

#include "Types.h"
#include "Helper.h"



struct HalfPosition {
	U32 bitboard;
	U32 possibleMoves;
	std::array<U16, 2 * 4 * 5> moveTable;
};

std::array<HalfPosition, fact(25, 21) / fact(4)> CardCombinationMoveList; // todo: this would only include 5 pawn moves, not 4 pawn etc

