#pragma once

#include "board.h"

#include "card.hpp"

#include <immintrin.h>
#include <x86intrin.h>



template <bool player>
bool inline Board::isTempleKingInRange() const{
	// player king can move to temple
	// return reverseMoveBoard[TEMPLE[player]] & k[player];
	return false;
}


template <bool player>
bool inline Board::isTempleFree() const {
	// no player piece is blocking the temple.
	return !(p[player] & (1 << TEMPLE[player]));
}


template <bool player>
bool inline Board::isTempleWinInOne() const {
	return isTempleKingInRange<player>() && isTempleFree<player>();
}


U32 inline Board::isKingAttackedBy(U32 bbk, U32 bbp) {
	U32 pk = _tzcnt_u32(bbk);
	// return reverseMoveBoard[pk] & bbp;
	return false;
}

// is !player king safe?
// is player attacking !players king?
template <bool player>
U32 inline Board::isKingAttacked(U32 bbk) const {
	return isKingAttackedBy(bbk, p[player]);
}



template <bool player>
U32 inline Board::isTakeWinInOne() const {
	return isKingAttacked<player>(k[!player]);
}

template <bool player>
bool inline Board::isWinInOne() const {
	if (isTempleWinInOne<player>())
		return true;
	return isTakeWinInOne<player>();
}



template <bool player>
void inline Board::doWinInOne() {
	if (isTempleWinInOne<player>()) {
		p[player] ^= k[player];
		k[player] = 1 << TEMPLE[player];
		p[!player] &= ~k[player];
	} else {
		U32 attacker = isKingAttacked<player>(k[!player]);
		attacker &= -attacker; // select first piece that can kill king
		p[player] ^= attacker ^ k[!player];
		k[!player] = 0;
		p[!player] ^= attacker;
	}
}