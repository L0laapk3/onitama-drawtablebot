#pragma once

#include "board.h"

#include "card.hpp"

#include <cassert>
#include <immintrin.h>
#include <x86intrin.h>



template <bool player>
bool inline Board::isTempleKingInRange(const MoveBoardList& moveList) const {
	// player king can move to temple
	return (moveList[TEMPLE[player]].flip[!player].cards[0] | moveList[TEMPLE[player]].flip[!player].cards[1]) & k[player];
}


template <bool player>
bool inline Board::isTempleFree() const {
	// no player piece is blocking the temple.
	return !(p[player] & (1 << TEMPLE[player]));
}


template <bool player>
bool inline Board::isTempleWinInOne(const MoveBoardList& moveList) const {
	return isTempleKingInRange<player>(moveList) && isTempleFree<player>();
}


template <bool player>
U32 inline Board::isKingAttackedBy(const MoveBoardList& moveList, U32 bbk, U32 bbp) {
	U32 pk = _tzcnt_u32(bbk);
	return (moveList[pk].flip[!player].cards[0] | moveList[pk].flip[!player].cards[1]) & bbp;
}

// is !player king safe?
// is player attacking !players king?
template <bool player>
U32 inline Board::isKingAttacked(const MoveBoardList& moveList, U32 bbk) const {
	return isKingAttackedBy<player>(moveList, bbk, p[player]);
}



template <bool player>
U32 inline Board::isTakeWinInOne(const MoveBoardList& moveList) const {
	return isKingAttacked<player>(moveList, k[!player]);
}

template <bool player>
bool inline Board::isWinInOne(const MoveBoardList& moveList) const {
	if (isTempleWinInOne<player>(moveList))
		return true;
	return isTakeWinInOne<player>(moveList);
}



template <bool player>
void inline Board::doWinInOne(const MoveBoardList& moveList) {
	assert(isWinInOne<player>(moveList));

	bool secondCard;
	if (isTakeWinInOne<player>(moveList)) {
		U32 attacker = isKingAttacked<player>(moveList, k[!player]);
		attacker &= -attacker; // select first piece that can kill king
		secondCard = moveList[_tzcnt_u32(k[!player])].flip[1].cards[1] & attacker;
		p[player] ^= attacker ^ k[!player];
		if (attacker & k[player])
			k[player] = k[!player];
		p[!player] ^= k[!player];
		k[!player] = 0;
	} else {
		secondCard = moveList[TEMPLE[player]].flip[1].cards[1] & k[player];
		p[player] ^= k[player] ^ (1 << TEMPLE[player]);
		k[player] = 1 << TEMPLE[player];
		p[!player] &= ~k[player];
	}
	cardI = CARDS_SWAP[cardI][player][secondCard];

	recalculateHash(!player);
}