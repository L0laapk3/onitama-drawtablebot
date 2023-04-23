#include "board.h"

#include <immintrin.h>
#include <x86intrin.h>



std::array<U32, 25> moveBoard0, moveBoard1; // TODO

// assumes p0 is to move
// assumes the board is not a win in 1
template<bool player>
inline void Board::iterateMoves(bool quiesence, const std::function<bool()> f) {

	U32 sourceBits = p[player];
	do {
		U32 sourcePiece = sourceBits & -sourceBits;
		sourceBits &= sourceBits - 1;
		U32 sourceIndex = _tzcnt_u32(sourcePiece);

		U32 landBits = moveBoard0[sourceIndex] | moveBoard1[sourceIndex];
		if (quiesence)
			landBits &= p[!player];

		p[player] ^= sourcePiece;
		U32 sourceKing = sourcePiece & k[player];
		k[player] ^= sourceKing;
		U32 kingMask = sourceKing ? 0 : ~0;

		while (landBits) {
			U32 landPiece = landBits & -landBits;
			landBits &= landBits - 1;
			U32 landIndex = _tzcnt_u32(landPiece);

			p[player] ^= landPiece;
			U32 takenPiece = p[!player] & landPiece; // opt: quiescence
			p[!player] ^= takenPiece;
			k[player] ^= (landPiece & kingMask);


        	if (!f) std::unreachable();
			bool stop = !f();

			p[player] |= landPiece;
			p[!player] |= takenPiece;
			k[player] |= (landPiece & kingMask);

			if (stop) {
				sourceBits = 0;
				break;
			}
		}

		p[player] |= sourcePiece;
		k[player] |= sourceKing;
	} while (sourceBits);
}