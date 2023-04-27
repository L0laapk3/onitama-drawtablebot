#pragma once

#include "types.h"
#include "card.hpp"
#include "xoshiro256.h"

#include <array>


struct Zobrist {
	U64 turn;
	std::array<std::array<U64, 25>, 3> pawns; // third dimension: piece takes pawn
	std::array<std::array<U64, 25>, 4> kings; // third dimension: p0 takes p1 pawn, fourth dimension: p1 takes p0 pawn
	std::array<std::array<std::array<U64, 2>, 2>, 30> moves;
	std::array<U64, 30> cards;
};


constexpr auto GENERATE_ZOBRIST() { // from random.org
	Zobrist zobrist;
	Xoshiro256 rand{ 0xe9ed6df459a4f6dbULL, 0xfa8a8a984046e29eULL, 0xdd3378009b2c517cULL, 0xd084f48fb3057748ULL };

	zobrist.turn = rand.next();

	zobrist.pawns = { 0 };
	for (int i = 0; i < 2; i++) {
		for (auto& pawn : zobrist.pawns[i])
			pawn = rand.next();
		for (auto& king : zobrist.kings[i])
			king = rand.next();
	}
	for (int i = 0; i < 25; i++) {
		zobrist.pawns[2][i] = zobrist.pawns[0][i] ^ zobrist.pawns[1][i];
		zobrist.kings[2][i] = zobrist.kings[0][i] ^ zobrist.pawns[1][i];
		zobrist.kings[3][i] = zobrist.kings[1][i] ^ zobrist.pawns[0][i];
	}

	for (auto& card : zobrist.cards)
		card = rand.next();
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
				zobrist.moves[i][j][k] = zobrist.cards[i] ^ zobrist.cards[CARDS_SWAP[i][j][k]];

	return zobrist;
};

constexpr auto ZOBRIST = GENERATE_ZOBRIST();
