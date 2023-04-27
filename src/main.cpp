#include "board.h"
#include "boardSearch.hpp"
#include "card.hpp"
#include "connection.h"
#include "game.h"
#include "test/testMain.h"

#include <iostream>



void singleSearch() {
	auto& cards = CARDS_PERFT;
	Board board = Board::create(0, { 0b00000'00000'00000'01110'00000, 0b00000'01110'00000'00000'00000 }, { 0b00000'00000'00000'00100'00000, 0b00000'00100'00000'00000'00000 });
	// Board board = Board::create({ 0b00000'00000'00000'01110'00000, 0b00000'00000'00000'00000'00100 }, { 0b00000'00000'00000'00100'00000, 0b00000'00000'00000'00000'00100 });
	board.search(cards, 14);
}



void selfPlay() {
	auto& cards = CARDS_PERFT;

	bool player = 1;
	Board board = Board::create(player);
	std::vector<Board> boards{};
	std::vector<bool> players{};

	while (true) {
		boards.push_back(board);
		players.push_back(player);

		std::cout << (player ? "0" : "X") << ": ";
		const auto& result = board.searchTime(cards, 50, player);
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
}



void onlinePlay(int argc, char** argv) {
	auto conn = Connection();
	if (argc > 1)
		conn.sendJoin(argv[1]);
	else
		conn.sendCreate();

	Game game(conn);
	game.board.assertValid(*game.cards, !game.myTurn);

	std::cout << (game.player ? "red" : "blue") << std::endl;

	while (true) {
		// game.board.print(*game.cards, game.myTurn);
		if (game.myTurn) {
			auto result = game.board.searchTime(*game.cards, 50);
			// result.board.print(*game.cards, game.myTurn);
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

	if (0) {
		singleSearch();
		return 0;
	}

	if (0) {
		selfPlay();
		return 0;
	}

	if (1) {
		onlinePlay(argc, argv);
		return 0;
	}
}
