#include "board.h"
#include "boardSearch.hpp"
#include "card.hpp"
#include "connection.h"
#include "game.h"

#include <iostream>


int main(int argc, char** argv) {

	if (1) {
		auto& cards = CARDS_PERFT;

		Board board = Board::create();
		board.p[1] = board.k[1];

		while (true) {
			board.print();
			auto result = board.search<0>(cards.moveBoards, 5);
			std::cout << "Result: " << result.score << std::endl;
			board = result.board;

			board.print();
			result = board.search<1>(cards.moveBoards, 5);
			std::cout << "Result: " << result.score << std::endl;
			board = result.board;
		}
		return 0;
	}



	auto conn = Connection();
	if (argc > 1)
		conn.sendJoin(argv[1]);
	else
		conn.sendCreate();

	Game game(conn);

	while (true) {
		game.board.print();
		if (game.myTurn) {
			// auto bestMove = game.searchTime(game.board, 10000, 2);
			auto result = game.board.search<0>(game.cards->moveBoards, 1);
			std::cout << "Result: " << result.score << std::endl;
			result.board.print();
			conn.submitMove(game, result.board, 0);
		}

		conn.waitTurn(game);
		if (game.ended) {
			std::cout << (!game.myTurn ? "won" : "lost") << std::endl;
			break;
		}
	}

	return 0;
}
