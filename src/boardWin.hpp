#pragma once

#include "board.h"

#include "card.hpp"

#include <cassert>
#include <immintrin.h>
#include <x86intrin.h>



template <bool player>
bool inline Board::isTempleKingInRange(const std::array<const U32*, 2>& reverseMoveBoards) const {
	// player king can move to temple
	return (reverseMoveBoards[0][TEMPLE[player]] | reverseMoveBoards[1][TEMPLE[player]]) & k[player];
}


template <bool player>
bool inline Board::isTempleFree() const {
	// no player piece is blocking the temple.
	return !(p[player] & (1 << TEMPLE[player]));
}


template <bool player>
bool inline Board::isTempleWinInOne(const std::array<const U32*, 2>& reverseMoveBoards) const {
	return isTempleKingInRange<player>(reverseMoveBoards) && isTempleFree<player>();
}


U32 inline Board::isKingAttackedBy(const std::array<const U32*, 2>& reverseMoveBoards, U32 bbk, U32 bbp) {
	U32 pk = _tzcnt_u32(bbk);
	return (reverseMoveBoards[0][pk] | reverseMoveBoards[1][pk]) & bbp;
}

// is !player king safe?
// is player attacking !players king?
template <bool player>
U32 inline Board::isKingAttacked(const std::array<const U32*, 2>& reverseMoveBoards, U32 bbk) const {
	return isKingAttackedBy(reverseMoveBoards, bbk, p[player]);
}



template <bool player>
U32 inline Board::isTakeWinInOne(const std::array<const U32*, 2>& reverseMoveBoards) const {
	return isKingAttacked<player>(reverseMoveBoards, k[!player]);
}

template <bool player>
bool inline Board::isWinInOne(const std::array<const U32*, 2>& reverseMoveBoards) const {
	if (isTempleWinInOne<player>(reverseMoveBoards))
		return true;
	return isTakeWinInOne<player>(reverseMoveBoards);
}



template <bool player>
void inline Board::doWinInOne(const std::array<const U32*, 2>& reverseMoveBoards) {
	assert(isWinInOne<player>(reverseMoveBoards));

	bool secondCard;
	if (isTakeWinInOne<player>(reverseMoveBoards)) {
		U32 attacker = isKingAttacked<player>(reverseMoveBoards, k[!player]);
		attacker &= -attacker; // select first piece that can kill king
		secondCard = reverseMoveBoards[1][_tzcnt_u32(k[!player])] & attacker;
		p[player] ^= attacker ^ k[!player];
		if (attacker & k[player])
			k[player] = k[!player];
		p[!player] ^= k[!player];
		k[!player] = 0;
	} else {
		secondCard = reverseMoveBoards[1][TEMPLE[player]] & k[player];
		p[player] ^= k[player] ^ (1 << TEMPLE[player]);
		k[player] = 1 << TEMPLE[player];
		p[!player] &= ~k[player];
	}
	cardI = CARDS_SWAP[cardI][player][secondCard];
	assertValid(true);
}