#include "board.h"

#include <cassert>
#include <immintrin.h>
#include <x86intrin.h>



std::array<U32, 25> moveBoard0, moveBoard1; // TODO

// assumes p0 is to move
// assumes the board is not a win in 1
template<bool player>
inline void Board::iterateMoves(const CardsInfo& cards, bool quiesence, const std::function<bool()> f) {
	Board beforeBoard = *this;

	auto permutation = CARDS_PERMUTATIONS[cardI];
	U8 thisCardI = cardI;

	const std::array<const U32*, 2> moveBoard_cards{
		&cards.moveBoards[permutation.playerCards[player][0]][0][0],
		&cards.moveBoards[permutation.playerCards[player][1]][0][0],
	};

	bool cont = true;
	U32 sourceBits = p[player];
	do {
		U32 sourcePiece = sourceBits & -sourceBits;
		sourceBits &= sourceBits - 1;
		U32 sourceIndex = _tzcnt_u32(sourcePiece);

		U32 landBits = moveBoard_cards[0][sourceIndex] | moveBoard_cards[1][sourceIndex];
		landBits &= ~p[player];
		if (quiesence)
			landBits &= p[!player];

		p[player] ^= sourcePiece;
		U32 sourceKing = sourcePiece & k[player];
		k[player] ^= sourceKing;
		U32 kingMask = sourceKing ? ~0 : 0;

		while (landBits && cont) {
			U32 landPiece = landBits & -landBits;
			landBits &= landBits - 1;
			U32 landIndex = _tzcnt_u32(landPiece);

			p[player] |= landPiece;
			U32 takenPiece = landPiece & p[!player]; // opt:  `& p[!player]` not needed for quiescence
			p[!player] ^= takenPiece;
			k[player] |= (landPiece & kingMask);

			bool other = false, stop = false;
			#pragma unroll
			for (int i = 0; i < 2; i++) {
				if (other || (landPiece & moveBoard_cards[i][sourceIndex])) {

					cardI = CARDS_SWAP[thisCardI][player][i];

					if (!f) std::unreachable();
					cont = f();
					if (!cont)
						break;
				} else
					other = true;
			}

			p[player] ^= landPiece;
			p[!player] |= takenPiece;
			k[player] ^= (landPiece & kingMask);
		}

		p[player] |= sourcePiece;
		k[player] |= sourceKing;
		cardI = thisCardI;
	} while (sourceBits && cont);

	assert(beforeBoard == *this);
}