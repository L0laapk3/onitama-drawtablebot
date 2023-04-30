#include "board.h"
#include "util.h"
#include "zobrist.h"
#include "transpositionTable.h"

#include <immintrin.h>
#include <x86intrin.h>



// assumes p0 is to move
// assumes the board is not a win in 1
template<bool player, typename Callable>
inline void Board::iterateMoves(Game& game, bool quiesence, TranspositionMove bestMove, const Callable f) {
	Board beforeBoard = *this;
	U8 thisCardI = cardI;

	hash ^= ZOBRIST.turn;

	Board beforeBoardFlipped = *this;

	U32 pBest = 0;
	U32 pFrom = 1U << bestMove.fromBitFull;
	U32 pTo = 1U << bestMove.toBit;
	U32 pMove = pFrom | pTo;
	if (bestMove.full) {
		p[player] ^= pMove;
		pBest = p[player];
		U32 kMove = pFrom & k[player] ? pMove : 0;
		k[player] ^= kMove;
		U32 pRemoved = p[!player] & pTo;
		p[!player] ^= pRemoved;

		U64 beforeHash = hash;
		if (kMove)
			hash ^= ZOBRIST.kings[player][bestMove.fromBitFull] ^ ZOBRIST.kings[pRemoved ? 2 + player : player][bestMove.toBit];
		else
			hash ^= ZOBRIST.pawns[player][bestMove.fromBitFull] ^ ZOBRIST.pawns[pRemoved ? 2 : player][bestMove.toBit];

		hash ^= ZOBRIST.moves[cardI][player][bestMove.secondCard];
		cardI = CARDS_SWAP[cardI][player][bestMove.secondCard];

		assertValid(*game.cards, !player);
		Board beforeBoard2 = *this;
		bool cont = f(bestMove);
		assume(*this == beforeBoard2);

		cardI = thisCardI;

		p[player] ^= pMove;
		k[player] ^= kMove;

		p[!player] |= pRemoved;

		hash = beforeHash;

		assume(*this == beforeBoardFlipped);

		if (!cont)
			goto end;
	}

	{
		const auto& moveList = game.cards->moveBoards[CARDS_HAND[player][cardI]];

		bool cont = true;
		#pragma unroll
		for (int quiet = 0; quiet < 2; quiet++) {
			U32 sourceBits = p[player];
			do {
				TranspositionMove ttMove;

				U32 sourcePiece = sourceBits & -sourceBits;
				sourceBits &= sourceBits - 1;
				ttMove.fromBitFull = _tzcnt_u32(sourcePiece);
				auto& landBitsCards = moveList[ttMove.fromBitFull].flip[player].cards;
				U32 landBits = landBitsCards[0] | landBitsCards[1];

				landBits &= ~p[player];
				landBits &= quiet ? ~p[!player] : p[!player];

				p[player] ^= sourcePiece;
				U32 sourceKing = sourcePiece & k[player];
				k[player] ^= sourceKing;
				U32 kingMask = sourceKing ? ~0 : 0;

				hash ^= kingMask ? ZOBRIST.kings[player][ttMove.fromBitFull] : ZOBRIST.pawns[player][ttMove.fromBitFull];

				while (landBits && cont) {
					U32 landPiece = landBits & -landBits;
					landBits &= landBits - 1;
					ttMove.toBitFull = _tzcnt_u32(landPiece);

					p[player] |= landPiece;
					if (!quiet)
						p[!player] ^= landPiece;
					k[player] |= landPiece & kingMask;

					hash ^= kingMask ? ZOBRIST.kings[quiet ? player : 2 + player][ttMove.toBitFull] : ZOBRIST.pawns[quiet ? player : 2][ttMove.toBitFull]; // quiet: combined take hash

					bool other = false, stop = false;
					#pragma unroll
					for (int i = 0; i < 2; i++) {
						if (other || (landPiece & landBitsCards[i])) {

							if (p[player] != pBest || bestMove.secondCard != i) [[likely]] {
								cardI = CARDS_SWAP[thisCardI][player][i];
								hash ^= ZOBRIST.moves[thisCardI][player][i];

								if (i)
									ttMove.secondCard = true;

								assertValid(*game.cards, !player);
								Board beforeBoard2 = *this;
								cont = f(ttMove);
								assume(*this == beforeBoard2);

								hash ^= ZOBRIST.moves[thisCardI][player][i];

								if (!cont)
									break;
							}
						} else
							other = true; // one of two cards must be used
					}

					hash ^= kingMask ? ZOBRIST.kings[quiet ? player : 2 + player][ttMove.toBit] : ZOBRIST.pawns[quiet ? player : 2][ttMove.toBit];

					p[player] ^= landPiece;
					if (!quiet)
						p[!player] |= landPiece;
					k[player] ^= landPiece & kingMask;
				}

				hash ^= kingMask ? ZOBRIST.kings[player][ttMove.fromBit] : ZOBRIST.pawns[player][ttMove.fromBit];

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
	}
end:
	hash ^= ZOBRIST.turn;

	assume(*this == beforeBoard);
}