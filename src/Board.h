#pragma once

#include "Types.h"

#include <array>



class Board {
    std::array<U32, 2> p;
    std::array<U32, 2> k;
	U8 cardsIndex;
};