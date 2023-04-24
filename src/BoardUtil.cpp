#include "board.h"
#include <iostream>
#include <bitset>
#include <immintrin.h>
#include <x86intrin.h>
#include <cassert>



Board Board::create(const CardsInfo& cards, std::array<U32, 2> p, std::array<U32, 2> k) {
	return Board{ p, k };
}


void Board::checkValid(bool isWon) const {
	auto test = [&](bool result) {
		if (!result) {
			std::cout << "Invalid board!" << std::endl;
			print();
		assert(false);
		}
	};

	test((p[0] & ((1 << 25) - 1)) == p[0]);
	test((p[1] & ((1 << 25) - 1)) == p[1]);
	test(_popcnt32(p[0]) <= 5);
	test(_popcnt32(p[1]) <= 5);
	if (!isWon) {
		test(1 <= _popcnt32(p[0]));
		test(1 <= _popcnt32(p[1]));
		test(_popcnt32(k[0]) == 1);
		test(_popcnt32(k[1]) == 1);
	} else {
		test(_popcnt32(k[0]) <= 1);
		test(_popcnt32(k[1]) <= 1);
	}
	test(k[0] == (p[0] & k[0]));
	test(k[1] == (p[1] & k[1]));
	test((p[0] & p[1]) == 0);
	test(cardI < 30);
}

void Board::assertValid(bool isWon) const {
#ifndef NDEBUG
	checkValid(isWon);
#endif
}

void Board::print() const {
	for (int r = 0; r < 5; r++) {
		for (int c = 5; c --> 0;) {
			const int mask = 1 << (5 * r + c);
			if (p[0] & mask) {
				if ((p[1] | k[1]) & mask)    std::cout << '?'; // invalid
				else if (k[0] & mask)        std::cout << 'X';
				else                         std::cout << '+';
			} else if (p[1] & mask) {
				if (k[1] & mask)             std::cout << 'O';
				else                         std::cout << 'o';
			} else if ((k[0] | k[1]) & mask) std::cout << '!'; // invalid
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


bool Board::operator==(const Board& other) const {
	return p[0] == other.p[0] && p[1] == other.p[1] && k[0] == other.k[0] && k[1] == other.k[1] && cardI == other.cardI;
}