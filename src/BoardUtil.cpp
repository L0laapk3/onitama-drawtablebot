#include "board.h"
#include <iostream>
#include <bitset>





Board Board::create(const CardsInfo& cards, std::array<U32, 2> p, std::array<U32, 2> k) {
	return Board{ p, k };
}


void Board::print() const {
	for (int r = 5; r-- > 0;) {
		for (int c = 0; c < 5; c++) {
			const int mask = 1 << (5 * r + c);
			if (p[0] & mask) {
				if ((p[1] | k[1]) & mask)    std::cout << '?'; // invalid
				else if (k[0] & mask)        std::cout << '0';
				else                         std::cout << 'o';
			} else if (p[1] & mask) {
				if (k[1] & mask)             std::cout << 'X';
				else                         std::cout << '+';
			} else if ((k[0] | k[1]) & mask) std::cout << 'K'; // invalid
			else                             std::cout << '.';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}


Board Board::invert() const {
	Board board{};
	for (U64 kI = 0; kI < 2; kI++) {
		U64 pPrev = p[kI];
		U64 kPrev = k[kI];
		for (U64 i = 0; i < 25; i++) {
			board.p[1-kI] = (board.p[1-kI] << 1) + (pPrev & 1);
			pPrev >>= 1;
			board.k[1-kI] = (board.k[1-kI] << 1) + (kPrev & 1);
			kPrev >>= 1;
		}
	}
	return board;
}