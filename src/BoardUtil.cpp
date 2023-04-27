#include "board.h"
#include "zobrist.h"

#include <iostream>
#include <bitset>
#include <immintrin.h>
#include <x86intrin.h>
#include <cassert>



Board Board::create(std::array<U32, 2> p, std::array<U32, 2> k) {
	return Board{ p, k };
}


void Board::checkValid(const CardsInfo& cards, bool isWon) const {
	auto test = [&](bool result) {
		if (!result) {
			std::cout << "Invalid board!" << std::endl;
			print(cards);
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

void Board::assertValid(const CardsInfo& cards, bool isWon) const {
#ifndef NDEBUG
	checkValid(cards, isWon);
#endif
}

std::string cardsShortName(Card card, int length) {
	std::string res = "";
	for (U32 i = 0; i < length; i++)
		res += card.name.size() > i ? card.name[i] : ' ';
	return res;
}
void Board::print(const CardsInfo& cards, char turnIndicator) const {
	auto perm = CARDS_PERMUTATIONS[cardI];

	std::cout << std::endl << cardsShortName(cards.cards[perm.playerCards[0][0]], 4) << ' ' << cardsShortName(cards.cards[perm.playerCards[0][1]], 4) << std::endl;

	auto sideCard = cardsShortName(cards.cards[perm.sideCard], 5);
	for (int r = 0; r < 5; r++) {
		std::cout << (r == 4 ? turnIndicator : ' ') << '|';

		for (int c = 5; c --> 0;) {
			const int mask = 1 << (5 * r + c);
			if (p[0] & mask) {
				if ((p[1] | k[1]) & mask)    std::cout << '?'; // invalid
				else if (k[0] & mask)        std::cout << 'X';
				else                         std::cout << '+';
			} else if (p[1] & mask) {
				if (k[1] & mask)             std::cout << '0';
				else                         std::cout << 'o';
			} else if ((k[0] | k[1]) & mask) std::cout << '!'; // invalid
			else                             std::cout << '.';
		}
		std::cout << '|' << sideCard[r] << std::endl;
	}
	std::cout << cardsShortName(cards.cards[perm.playerCards[1][0]], 4) << ' ' << cardsShortName(cards.cards[perm.playerCards[1][1]], 4) << std::endl << std::endl;
}
void Board::print(const CardsInfo& cards) const {
	print(cards, ' ');
};
void Board::print(const CardsInfo& cards, bool player) const {
	print(cards, player ? 'X' : '0');
};


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


void Board::recalculateHash(bool player) {
	hash = 0;
	for (U64 player = 0; player < 2; player++) {
		U32 sourceBits = p[player];
		while (sourceBits) {
			U32 sourcePiece = sourceBits & -sourceBits;
			sourceBits &= sourceBits - 1;
			U32 sourceIndex = _tzcnt_u32(sourcePiece);
			hash ^= sourcePiece & k[player] ? ZOBRIST.kings[player][sourceIndex] : ZOBRIST.pawns[player][sourceIndex];
		}
	}

	if (player)
		hash ^= ZOBRIST.turn;

	hash ^= ZOBRIST.cards[cardI];
}