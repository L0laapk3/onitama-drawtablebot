#include "board.h"
#include "zobrist.h"

#include <iostream>
#include <bitset>
#include <immintrin.h>
#include <x86intrin.h>
#include <cassert>



Board Board::create(bool player, std::array<U32, 2> p, std::array<U32, 2> k) {
	Board board{ p, k };
	board.recalculateHash(player);
	return board;
}


void Board::checkValid(const CardsInfo& cards, bool player, bool isWon) const {
	auto test = [&](bool result) {
		if (!result) {
			std::cout << "Invalid board!" << std::endl;
			print(cards);
			assert(false);
			std::exit(1);
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

	Board tmpBoard = *this;
	tmpBoard.recalculateHash(player);
	test(tmpBoard.hash == hash);
}

void Board::assertValid(const CardsInfo& cards, bool player, bool isWon) const {
#ifndef NDEBUG
	checkValid(cards, player, isWon);
#endif
}

std::string cardsShortName(Card card, int length) {
	std::string res = "";
	for (U32 i = 0; i < length; i++)
		res += card.name.size() > i ? card.name[i] : ' ';
	return res;
}

std::string Board::toString(const CardsInfo& cards, std::vector<Board> boards, std::vector<char> turnIndicators) {
	std::string outString{};
	constexpr int BOARDS_PER_LINE = 8;

	for (int i = 0; i < boards.size(); i += BOARDS_PER_LINE) {
		outString += "\n";
		for (int j = i; j < i + BOARDS_PER_LINE && j < boards.size(); j++) {
			auto perm = CARDS_PERMUTATIONS[boards[j].cardI];
			outString += cardsShortName(cards.cards[perm.playerCards[0][0]], 4) + " " + cardsShortName(cards.cards[perm.playerCards[0][1]], 4) + "  ";
		}
		outString += "\n";

		for (int r = 0; r < 5; r++) {
			for (int j = i; j < i + BOARDS_PER_LINE && j < boards.size(); j++) {
				auto& board = boards[j];
				auto perm = CARDS_PERMUTATIONS[board.cardI];
				outString += std::string(1, r == 4 ? turnIndicators[j] : ' ') + "|";

				for (int c = 5; c --> 0;) {
					const int mask = 1 << (5 * r + c);
					if (board.p[0] & mask) {
						if ((board.p[1] | board.k[1]) & mask)    outString += "?"; // invalid
						else if (board.k[0] & mask)              outString += "X";
						else                                     outString += "+";
					} else if (board.p[1] & mask) {
						if (board.k[1] & mask)                   outString += "0";
						else                                     outString += "o";
					} else if ((board.k[0] | board.k[1]) & mask) outString += "!"; // invalid
					else                                         outString += ".";
				}
				outString += "|" + std::string(1, cardsShortName(cards.cards[perm.sideCard], 5)[r]) + "  ";
			}
			outString += "\n";
		}
		for (int j = i; j < i + BOARDS_PER_LINE && j < boards.size(); j++) {
			auto perm = CARDS_PERMUTATIONS[boards[j].cardI];
			outString += cardsShortName(cards.cards[perm.playerCards[1][0]], 4) + " " + cardsShortName(cards.cards[perm.playerCards[1][1]], 4) + "  ";
		}
		outString += "\n";
	}
	outString += "\n";
	return outString;
}
std::string Board::toString(const CardsInfo& cards, std::vector<Board> boards, std::vector<bool> players) {
	std::vector<char> turnIndicators(players.size());
	for (int i = 0; i < players.size(); i++)
		turnIndicators[i] = players[i] ? '0' : 'X';
	return toString(cards, boards, turnIndicators);
}
std::string Board::toString(const CardsInfo& cards, std::vector<Board> boards) {
	std::vector<char> turnIndicators(boards.size(), ' ');
	return toString(cards, boards, turnIndicators);
}
std::string Board::toString(const CardsInfo& cards, bool player) const {
	return Board::toString(cards, { *this }, std::vector<bool>{ player });
};
std::string Board::toString(const CardsInfo& cards) const {
	return Board::toString(cards, { *this });
};
void Board::print(const CardsInfo& cards) const {
	auto str = toString(cards);
	std::cout << str << std::flush;
};
void Board::print(const CardsInfo& cards, bool player) const {
	auto str = toString(cards, player);
	std::cout << str << std::flush;
};


Board Board::invert(bool player) const {
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
	board.cardI = CARDS_INVERT[cardI];
	board.recalculateHash(!player);
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