#include "board.h"
#include "boardSearch.hpp"
#include "card.hpp"
#include "connection.h"
#include "game.h"
#include "test/testMain.h"
#include "score.h"

#include <iostream>



void singleSearch() {
	Game game(
		&CARDS_PERFT,
		Board::create(0, { 0b00000'00000'00000'01110'00000, 0b00000'01110'00000'00000'00000 }, { 0b00000'00000'00000'00100'00000, 0b00000'00100'00000'00000'00000 })
	);

	// before aspiration windows: 6700ms
	// after aspiration windows: 4000ms

	game.board.print(*game.cards, 0);
	SearchResult result = game.searchTime({ .depth = 17 });
	result.board.print(*game.cards, 1);
}



void selfPlay(S64 timeMs) {
	auto cards = randomDeck(); // CARDS_PERFT;
	Game game(&cards);
	game.board.print(*game.cards);

	std::vector<Board> boards{};
	std::vector<bool> players{};

	SearchPersistent persistent{};
	while (true) {
		boards.push_back(game.board);
		players.push_back(game.player);

		std::cout << (game.player ? "0" : "X") << ": ";
		const auto& result = game.searchTime({ .time = timeMs }, persistent);
		// result.board.print(*game.cards, game.player);
		game.board = result.board;
		game.player = !game.player;
		game.board.recalculateHash(game.player);

		std::swap(persistent.alpha, persistent.beta);
		persistent.alpha = -persistent.alpha;
		persistent.beta  = -persistent.beta;
		persistent.lastScore = -result.score; // negate because simulation both players

		if (result.winningMove)
			break;
		if (!result.foundMove) {
			std::cerr << "didnt find any moves" << std::endl;
			break;
		}
	}
	boards.push_back(game.board);
	players.push_back(game.player);
	// std::cout << Board::toString(*game.cards, boards, players);
}



void onlinePlay(int argc, char** argv, S64 timeMs) {
	auto conn = Connection();
	if (argc > 1)
		conn.sendJoin(argv[1]);
	else
		conn.sendCreate();

	Game game(conn);

	std::cout << (game.player ? "blue" : "red") << std::endl;

	SearchPersistent persistent{};
	while (true) {
		// game.board.print(*game.cards, !game.myTurn);
		if (game.myTurn) {
			auto result = game.searchTime({ .time = timeMs }, persistent);
			// result.board.print(*game.cards, game.myTurn);
			result.board.checkValid(*game.cards, game.myTurn, result.winningMove);
			game.submitMove(conn, result.board);

			persistent.lastScore = result.score;
			persistent.lastDepth--; // skip the opponent move
		} else {
			// std:: cout << "-" << std::endl;
		}

		game.waitTurn(conn);
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

	S64 timeMs = 100;

	if (1) {
		while (true)
			selfPlay(timeMs);
		return 0;
	}

	if (1) {
		onlinePlay(argc, argv, timeMs);
		return 0;
	}
}
