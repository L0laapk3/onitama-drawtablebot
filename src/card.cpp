#include "card.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>


const Card& findCard(const std::string_view& name) {
	return *std::find_if(ALL_CARDS_BY_NAME.begin(), ALL_CARDS_BY_NAME.end(), [&](const Card& card) { return card.name == name; });
}


void print(const MoveBoard& moves) {
	for (U64 j = 0; j < 25; j += 5) {
		for (int r = 0; r < 5; r++) {
			for (U64 i = j + 5; i --> j;) {
				for (int c = 0; c < 5; c++) {
					const int mask = 1 << (5 * r + c);
					std::cout << ((moves[i] & mask) ? 'o' : '-');
				}
				std::cout << ' ';
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}



constexpr CardsInfo CARDS_PERFT = CardsInfo::create({ BOAR, OX, ELEPHANT, HORSE, CRAB });



CardsInfo randomDeck() {
    std::random_device rd;
	std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, ALL_CARDS_BY_NAME.size() - 1);

	std::array<int, 5> cardIndices{ -1 };
	for (int i = 0; i < 5; i++) {
		do {
			cardIndices[i] = dist(gen);
		} while (std::find(cardIndices.begin(), cardIndices.begin() + i, cardIndices[i]) != cardIndices.begin() + i);
	}

	return CardsInfo::create({
		ALL_CARDS_BY_NAME[cardIndices[0]],
		ALL_CARDS_BY_NAME[cardIndices[1]],
		ALL_CARDS_BY_NAME[cardIndices[2]],
		ALL_CARDS_BY_NAME[cardIndices[3]],
		ALL_CARDS_BY_NAME[cardIndices[4]],
	});
}