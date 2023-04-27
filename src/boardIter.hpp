#include "board.h"
#include "util.h"
#include "zobrist.h"

#include <immintrin.h>
#include <x86intrin.h>



// assumes p0 is to move
// assumes the board is not a win in 1
template<bool player, typename Callable>
inline void Board::iterateMoves(const Game& game, bool quiesence, const Callable f) {
	Board beforeBoard = *this;

	hash ^= ZOBRIST.turn;

	U8 thisCardI = cardI;
	const auto& moveList = game.cards->moveBoards[CARDS_HAND[player][cardI]];

	bool cont = true;
	#pragma unroll
	for (int quiet = 0; quiet < 2; quiet++) {
		U32 sourceBits = p[player];
		do {
			U32 sourcePiece = sourceBits & -sourceBits;
			sourceBits &= sourceBits - 1;
			U32 sourceIndex = _tzcnt_u32(sourcePiece);
			auto& landBitsCards = moveList[sourceIndex].flip[player].cards;
			U32 landBits = landBitsCards[0] | landBitsCards[1];

			landBits &= ~p[player];
			landBits &= quiet ? ~p[!player] : p[!player];

			p[player] ^= sourcePiece;
			U32 sourceKing = sourcePiece & k[player];
			k[player] ^= sourceKing;
			U32 kingMask = sourceKing ? ~0 : 0;

			hash ^= kingMask ? ZOBRIST.kings[player][sourceIndex] : ZOBRIST.pawns[player][sourceIndex];

			while (landBits && cont) {
				U32 landPiece = landBits & -landBits;
				landBits &= landBits - 1;
				U32 landIndex = _tzcnt_u32(landPiece);

				p[player] |= landPiece;
				if (!quiet)
					p[!player] ^= landPiece;
				k[player] |= landPiece & kingMask;

				hash ^= kingMask ? ZOBRIST.kings[quiet ? player : 2 + player][landIndex] : ZOBRIST.pawns[quiet ? player : 2][landIndex]; // quiet: combined take hash

				bool other = false, stop = false;
				#pragma unroll
				for (int i = 0; i < 2; i++) {
					if (other || (landPiece & landBitsCards[i])) {

						cardI = CARDS_SWAP[thisCardI][player][i];
						hash ^= ZOBRIST.moves[thisCardI][player][i];

						assertValid(*game.cards, !player);
						Board beforeBoard2 = *this;
						cont = f();
						assume(*this == beforeBoard2);
						assertValid(*game.cards, !player);

						hash ^= ZOBRIST.moves[thisCardI][player][i];

						if (!cont)
							break;
					} else
						other = true; // one of two cards must be used
				}

				hash ^= kingMask ? ZOBRIST.kings[quiet ? player : 2 + player][landIndex] : ZOBRIST.pawns[quiet ? player : 2][landIndex];

				p[player] ^= landPiece;
				if (!quiet)
					p[!player] |= landPiece;
				k[player] ^= landPiece & kingMask;
			}

			hash ^= kingMask ? ZOBRIST.kings[player][sourceIndex] : ZOBRIST.pawns[player][sourceIndex];

			p[player] |= sourcePiece;
			k[player] |= sourceKing;

#ifndef NDEBUG
			Board tmpBoard = *this;
			tmpBoard.cardI = thisCardI;
			tmpBoard.assertValid(*game.cards, !player);
#endif
		} while (sourceBits && cont);
		if (!cont || quiesence)
			break;
	}

	cardI = thisCardI;

	hash ^= ZOBRIST.turn;

	assume(*this == beforeBoard);
}