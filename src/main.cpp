#include "board.h"
#include "boardSearch.hpp"
#include "card.hpp"
#include "connection.h"
#include "game.h"
#include "test/testMain.h"

#include <iostream>



void singleSearch() {
	Game game(
		&CARDS_PERFT,
		Board::create(0, { 0b00000'00000'00000'01110'00000, 0b00000'01110'00000'00000'00000 }, { 0b00000'00000'00000'00100'00000, 0b00000'00100'00000'00000'00000 })
	); // before TT: 6400-6800ms

	game.board.search(game, 9);
	game.board.search(game, 10);
	game.board.search(game, 11);
	game.board.search(game, 12);
	game.board.search(game, 13);
	auto result = game.board.search(game, 14);
	result.board.print(*game.cards);
}



void selfPlay(S64 timeMs) {
	Game game(
		&CARDS_PERFT
	);

	std::vector<Board> boards{};
	std::vector<bool> players{};

	while (true) {
		boards.push_back(game.board);
		players.push_back(game.player);

		std::cout << (game.player ? "0" : "X") << ": ";
		const auto& result = game.board.searchTime(game, timeMs, game.player);
		game.board = result.board;
		game.player = !game.player;
		game.board.recalculateHash(game.player);

		if (result.winningMove)
			break;
		if (!result.foundMove) {
			std::cerr << "didnt find any moves" << std::endl;
			break;
		}
	}
	boards.push_back(game.board);
	players.push_back(game.player);
	std::cout << Board::toString(*game.cards, boards, players);
}



void onlinePlay(int argc, char** argv, S64 timeMs) {
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
			auto result = game.board.searchTime(game, timeMs);
			result.board.print(*game.cards, game.myTurn);
			result.board.checkValid(*game.cards, game.myTurn, result.winningMove);
			conn.submitMove(game, result.board, 0);
		} else
			std:: cout << "-" << std::endl;

		conn.waitTurn(game);
		if (game.ended) {
			std::cout << (!game.myTurn ? "won" : "lost") << std::endl;
			break;
		}
	}
}




int main(int argc, char** argv) {
	if (0)
		testMain();

	S64 timeMs = 100;

	if (0) {
		singleSearch();
		return 0;
	}

	if (1) {
		selfPlay(timeMs);
		return 0;
	}

	if (1) {
		onlinePlay(argc, argv, timeMs);
		return 0;
	}
}
