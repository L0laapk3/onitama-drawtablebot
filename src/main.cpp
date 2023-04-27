#include "board.h"
#include "boardSearch.hpp"
#include "card.hpp"
#include "connection.h"
#include "game.h"
#include "test/testMain.h"

#include <iostream>


int main(int argc, char** argv) {
	testMain();

	if (0) {
		auto& cards = CARDS_PERFT;
		Board board = Board::create(0, { 0b00000'00000'00000'01110'00000, 0b00000'01110'00000'00000'00000 }, { 0b00000'00000'00000'00100'00000, 0b00000'00100'00000'00000'00000 });
		// Board board = Board::create({ 0b00000'00000'00000'01110'00000, 0b00000'00000'00000'00000'00100 }, { 0b00000'00000'00000'00100'00000, 0b00000'00000'00000'00000'00100 });
		board.search(cards, 9);
		return 0;
	}


	if (1) {
		auto& cards = CARDS_PERFT;

		bool player = 1;
		Board board = Board::create(player);
		std::vector<Board> boards{};
		std::vector<bool> players{};

		while (true) {
			boards.push_back(board);
			players.push_back(player);

			std::cout << (player ? "0" : "X") << ": ";
			const auto& result = board.search(cards, 5, player);
			board = result.board;
			player = !player;
			board.recalculateHash(player);

			if (result.winningMove)
				break;
			if (!result.foundMove) {
				std::cerr << "didnt find any moves" << std::endl;
				break;
			}
		}
		boards.push_back(board);
		players.push_back(player);
		std::cout << Board::toString(cards, boards, players);
		return 0;
	}



	auto conn = Connection();
	if (argc > 1)
		conn.sendJoin(argv[1]);
	else
		conn.sendCreate();

	Game game(conn);
	game.board.assertValid(*game.cards, !game.myTurn);

	std::cout << (game.player ? "red" : "blue") << std::endl;

	while (true) {
		game.board.print(*game.cards, game.myTurn);
		if (game.myTurn) {
			auto result = game.board.searchTime(*game.cards, 50);
			conn.submitMove(game, result.board, 0);
		} else
			std:: cout << "-" << std::endl;

		conn.waitTurn(game);
		if (game.ended) {
			std::cout << (!game.myTurn ? "won" : "lost") << std::endl;
			break;
		}
	}

	return 0;
}
