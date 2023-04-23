#include "board.h"
#include "boardSearch.hpp"
#include "card.hpp"

#include <iostream>


int main(int, char**) {
	auto& cards = CARDS_PERFT;

	Board board = Board::create(cards);

	// board.print();
	// board.iterateMoves<0>(cards, false, [&]() {
	// 	board.print();
	// 	return true;
	// });
	// board.print();

	auto result = board.search<0>(cards, 1);
	std::cout << "Result: " << result.score << std::endl;
}
