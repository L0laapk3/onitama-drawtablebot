#include "board.h"
#include "boardSearch.hpp"
#include "card.hpp"
#include "connection.h"
#include "game.h"

#include <iostream>


int main(int argc, char** argv) {

	if (1) {
		auto& cards = CARDS_PERFT;
		Board board = Board::create({ 0b00000'00000'00000'01110'00000, 0b00000'01110'00000'00000'00000 }, { 0b00000'00000'00000'00100'00000, 0b00000'00100'00000'00000'00000 });
		// Board board = Board::create({ 0b00000'00000'00000'01110'00000, 0b00000'00000'00000'00000'00100 }, { 0b00000'00000'00000'00100'00000, 0b00000'00000'00000'00000'00100 });
		board.search(cards, 9);
		return 0;
	}


	if (0) {
		auto& cards = CARDS_PERFT;

		Board board = Board::create();
		board.p[1] = board.k[1];

		bool player = 0;
		while (true) {
			board.print(cards);
			const auto& result = board.search(cards, player, 5);
			std::cout << "Result: " << result.score << std::endl;
			board = result.board;
			player = !player;

			if (result.winningMove)
				return 0;
			if (!result.foundMove) {
				std::cerr << "didnt find any moves" << std::endl;
				return 1;
			}
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
		if (game.myTurn) {
			auto result = game.board.searchTime(*game.cards, 0, 1000);
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
