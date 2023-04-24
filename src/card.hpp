#pragma once

#include "types.h"

#include <array>
#include <functional>
#include <algorithm>
#include <string_view>



struct Card {
	const U32 bitboard;
	const std::string_view name;

	constexpr operator U32() const {
		return bitboard;
	}
};

constexpr auto BOAR     = Card{0b00000'00100'01010'00000'00000, "boar"};
constexpr auto COBRA    = Card{0b00000'01000'00010'01000'00000, "cobra"};
constexpr auto CRAB     = Card{0b00000'00100'10001'00000'00000, "crab"};
constexpr auto CRANE    = Card{0b00000'00100'00000'01010'00000, "crane"};
constexpr auto DRAGON   = Card{0b00000'10001'00000'01010'00000, "dragon"};
constexpr auto EEL      = Card{0b00000'00010'01000'00010'00000, "eel"};
constexpr auto ELEPHANT = Card{0b00000'01010'01010'00000'00000, "elephant"};
constexpr auto FROG     = Card{0b00000'00010'00001'01000'00000, "frog"};
constexpr auto GOOSE    = Card{0b00000'00010'01010'01000'00000, "goose"};
constexpr auto HORSE    = Card{0b00000'00100'00010'00100'00000, "horse"};
constexpr auto MANTIS   = Card{0b00000'01010'00000'00100'00000, "mantis"};
constexpr auto MONKEY   = Card{0b00000'01010'00000'01010'00000, "monkey"};
constexpr auto OX       = Card{0b00000'00100'01000'00100'00000, "ox"};
constexpr auto RABBIT   = Card{0b00000'01000'10000'00010'00000, "rabbit"};
constexpr auto ROOSTER  = Card{0b00000'01000'01010'00010'00000, "rooster"};
constexpr auto TIGER    = Card{0b00100'00000'00000'00100'00000, "tiger"};

constexpr std::array<Card, 16> ALL_CARDS_BY_NAME{ BOAR, COBRA, CRAB, CRANE, DRAGON, EEL, ELEPHANT, FROG, GOOSE, HORSE, MANTIS, MONKEY, OX, RABBIT, ROOSTER, TIGER };
constexpr std::array<U32, 16>  ALL_CARDS        { BOAR, COBRA, CRAB, CRANE, DRAGON, EEL, ELEPHANT, FROG, GOOSE, HORSE, MANTIS, MONKEY, OX, RABBIT, ROOSTER, TIGER };


const Card& findCard(const std::string_view& name);




struct CardPermutation {
	std::array<std::array<U8, 2>, 2> playerCards;
	U8 sideCard;
    std::array<U8, 3> _padding;
};




constexpr std::array<CardPermutation, 30> CARDS_PERMUTATIONS = {{
	{ 2, 3, 0, 1, 4 },
	{ 1, 2, 0, 4, 3 },
	{ 0, 3, 1, 2, 4 },
	{ 2, 4, 0, 1, 3 },
	{ 0, 2, 1, 3, 4 },
	{ 1, 4, 0, 2, 3 },
	{ 0, 3, 1, 4, 2 },
	{ 1, 2, 0, 3, 4 },
	{ 0, 4, 1, 2, 3 },
	{ 1, 4, 0, 3, 2 },
	{ 0, 1, 2, 3, 4 },
	{ 0, 2, 1, 4, 3 },
	{ 1, 3, 0, 2, 4 },
	{ 0, 1, 2, 4, 3 },
	{ 0, 1, 3, 4, 2 },
	{ 0, 2, 3, 4, 1 },
	{ 1, 2, 3, 4, 0 },
	{ 2, 4, 0, 3, 1 },
	{ 1, 3, 2, 4, 0 },
	{ 0, 4, 1, 3, 2 },
	{ 2, 3, 0, 4, 1 },
	{ 1, 4, 2, 3, 0 },
	{ 2, 3, 1, 4, 0 },
	{ 0, 4, 2, 3, 1 },
	{ 1, 3, 0, 4, 2 },
	{ 3, 4, 0, 2, 1 },
	{ 2, 4, 1, 3, 0 },
	{ 3, 4, 0, 1, 2 },
	{ 0, 3, 2, 4, 1 },
	{ 3, 4, 1, 2, 0 },
}};

// input cardI, output cardI
constexpr std::array<std::array<std::array<U8, 2>, 2>, 30> CARDS_SWAP = {{
	{ 27, 3,  22, 20 },
	{ 20, 24, 16, 7  },
	{ 29, 8,  28, 6  },
	{ 27, 0,  26, 17 },
	{ 26, 19, 15, 11 },
	{ 25, 12, 21, 9  },
	{ 22, 11, 28, 2  },
	{ 17, 9,  16, 1  },
	{ 29, 2,  23, 19 },
	{ 17, 7,  21, 5  },
	{ 21, 23, 14, 13 },
	{ 22, 6,  15, 4  },
	{ 25, 5,  18, 24 },
	{ 18, 28, 14, 10 },
	{ 16, 15, 13, 10 },
	{ 16, 14, 11, 4  },
	{ 15, 14, 1,  7  },
	{ 9,  7,  26, 3  },
	{ 28, 13, 24, 12 },
	{ 26, 4,  23, 8  },
	{ 24, 1,  22, 0  },
	{ 23, 10, 9,  5  },
	{ 6,  11, 20, 0  },
	{ 21, 10, 19, 8  },
	{ 20, 1,  18, 12 },
	{ 5,  12, 29, 27 },
	{ 19, 4,  17, 3  },
	{ 3,  0,  29, 25 },
	{ 18, 13, 6,  2  },
	{ 8,  2,  25, 27 },
}};


// input cardI, output cardI
constexpr std::array<U8, 30> CARDS_INVERT = { 10, 8, 7, 13, 12, 11, 9, 2, 1, 6, 0, 5, 4, 3, 27, 25, 29, 28, 26, 24, 23, 22, 21, 20, 19, 15, 18, 14, 17, 16 };


// output cardI
constexpr std::array<std::array<U8, 12>, 5> CARDS_USED_IN = {{
	{ 10, 4,  2,  13, 11, 8,  14, 6,  19, 15, 28, 23 },
	{ 10, 7,  12, 13, 1,  5,  14, 24, 9,  16, 18, 21 },
	{ 4,  7,  0,  11, 1,  3,  15, 20, 17, 16, 22, 26 },
	{ 2,  12, 0,  6,  24, 27, 28, 20, 25, 18, 22, 29 },
	{ 8,  5,  3,  19, 9,  27, 23, 17, 25, 21, 26, 29 },
}};


// output cardI
constexpr std::array<U8, 10> CARDS_P0_PAIRS = { 10, 4, 2, 7, 12, 0, 8, 5, 3, 27 };



typedef std::array<U32, 25> MoveBoard;

template<bool invert>
constexpr auto generateMoveBoard(const U32 card) {
    constexpr std::array<U32, 5> shiftMasks{
        0b11100'11100'11100'11100'11100,
        0b11110'11110'11110'11110'11110,
        0b11111'11111'11111'11111'11111,
        0b01111'01111'01111'01111'01111,
        0b00111'00111'00111'00111'00111,
    };
	U32 cardInverted = 0;
	for (U64 i = 0; i < 25; i++)
		cardInverted |= ((card >> i) & 1) << (invert ? 24 - i : i);

	MoveBoard moveBoard;
    for (U64 i = 0; i < 25; i++) {
        U32 maskedMove = cardInverted & shiftMasks[i % 5];
        moveBoard[i] = (i > 12 ? maskedMove << (i - 12) : maskedMove >> (12 - i)) & ((1ULL << 25) - 1);
    }
	return moveBoard;
}

constexpr auto combineMoveBoards(const MoveBoard& a, const MoveBoard& b) {
	MoveBoard comb;
	#pragma unroll
    for (U64 i = 0; i < 25; i++)
		comb[i] = a[i] | b[i];
	return comb;
}



typedef std::array<Card, 5> CardSet;

struct HalfPositionLocal { std::array<U32, 2> cards; };          // card0 and card1
struct PositionLocal { std::array<HalfPositionLocal, 2> flip; }; // forwards and reverses
typedef std::array<PositionLocal, 25> MoveBoardList;              // 25 position indices
typedef std::array<MoveBoardList, 10> MoveBoardSet;               // 5*4/2 half card combinations


constexpr auto generateMoveBoardSet(const CardSet& cards) {
	MoveBoardSet moveBoards;
	int cardI = 0;
	for (int card0 = 0; card0 < 5; card0++)
		for (int card1 = card0 + 1; card1 < 5; card1++) {
			auto& moveBoardOpt = moveBoards[cardI++];

			auto moveBoard0 = generateMoveBoard<false>(cards[card0]);
			auto moveBoard1 = generateMoveBoard<false>(cards[card1]);
			auto moveBoard0r = generateMoveBoard<true>(cards[card0]);
			auto moveBoard1r = generateMoveBoard<true>(cards[card1]);

			for (int pos = 0; pos < 25; pos++) {
				moveBoardOpt[pos].flip[0].cards[0] = moveBoard0[pos];
				moveBoardOpt[pos].flip[0].cards[1] = moveBoard1[pos];
				moveBoardOpt[pos].flip[1].cards[0] = moveBoard0r[pos];
				moveBoardOpt[pos].flip[1].cards[1] = moveBoard1r[pos];
			}
		}

    return moveBoards;
}

constexpr auto CARDS_HAND = [](){
	std::array<std::array<U8, 32>, 2> cardsHand{ (U8)-1 };
	for (int player = 0; player < 2; player++) {
		int cardI = 0;
		for (int card0 = 0; card0 < 5; card0++)
			for (int card1 = card0 + 1; card1 < 5; card1++) {
				for (int i = 0; i < 30; i++) {
					auto perm = CARDS_PERMUTATIONS[i];
					if (perm.playerCards[player][0] == card0 && perm.playerCards[player][1] == card1)
						cardsHand[player][i] = cardI;
				}
				cardI++;
			}
	}
	return cardsHand;
}();


struct CardsInfo {
	CardSet cards;
	MoveBoardSet moveBoards = generateMoveBoardSet(cards);
};



void print(const MoveBoard& moves);



const extern CardsInfo CARDS_PERFT;