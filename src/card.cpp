#include "card.hpp"

#include <iostream>
#include <vector>
#include <algorithm>


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



const CardsInfo CARDS_PERFT{ BOAR, OX, ELEPHANT, HORSE, CRAB };