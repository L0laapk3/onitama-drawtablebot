#include "board.h"
#include "boardSearch.hpp"
#include "card.hpp"
#include "connection.h"
#include "game.h"

#include <iostream>


int main(int argc, char** argv) {

	if (0) {
		auto& cards = CARDS_PERFT;

		Board board = Board::create(cards);

		auto result = board.search<0>(cards, 9);
		std::cout << "Result: " << result.score << std::endl;
		result.next.print();
		board = result.next;
		return 0;
	}



	auto conn = Connection();
	if (argc > 1)
		conn.sendJoin(argv[1]);
	else
		conn.sendCreate();

	Game game(conn);

	while (true) {
		game.board.invert().print();
		if (game.myTurn) {
			// auto bestMove = game.searchTime(game.board, 10000, 2);
			auto result = game.board.search<0>(*game.cards, 9);
			std::cout << "Result: " << result.score << std::endl;
			result.next.invert().print();
			conn.submitMove(game, result.next);
		}

		conn.waitTurn(game);
		if (conn.ended) {
			std::cout << "ended" << std::endl;
			break;
		}
	}

	return 0;
}
