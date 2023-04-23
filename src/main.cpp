#include "board.h"
#include "boardSearch.hpp"
#include "card.hpp"

#include <iostream>


int main(int, char**) {
	Board board = Board::create(CARDS_PERFT);

	auto result = board.search<0>(5);
	std::cout << "Result: " << result.score << std::endl;
}
