#include "game.h"

#include <cassert>



Board parseBoard(std::string str, bool flip, bool player, U8 cardI) {
	Board board{};
	for (int i = 0; i < 25; i++) {
		const auto c = str[flip ? 24ULL - i : i];
		if (c == '1' || c == '2') {
			board.p[flip] |= 1ULL << i;
			if (c == '2')
				board.k[flip] |= 1ULL << i;
		}
		if (c == '3' || c == '4') {
			board.p[!flip] |= 1ULL << i;
			if (c == '4')
				board.k[!flip] |= 1ULL << i;
		}
	}
	board.cardI = cardI; // 10 is the one with the cards in order 0, 1, 2, 3, 4
	board.recalculateHash(player);
	return board;
}

CardsInfo parseCards(std::array<std::string, 5> cardNames, bool flipped) {
    return CardsInfo::create({
        findCard(cardNames[flipped ? 2 : 0]),
        findCard(cardNames[flipped ? 3 : 1]),
        findCard(cardNames[flipped ? 0 : 2]),
        findCard(cardNames[flipped ? 1 : 3]),
        findCard(cardNames[4]),
    });
}




Game::Game(Connection& connection) : Game(connection.load()) { }

Game::Game(const Connection::LoadResult loadResult) : Game(loadResult.cardStr, loadResult.boardStr, loadResult.playerBlue, loadResult.redTurn) { }

// leak some memory, whatever
Game::Game(std::array<std::string, 5> cardNames, std::string boardString, bool player, bool p0Turn) :
	Game(new CardsInfo(parseCards(cardNames, player)), parseBoard(boardString, !player, player == p0Turn), player, player != p0Turn) { }

Game::Game(const CardsInfo* cards, const Board& board, bool player, bool myTurn) : cards(cards), board(board), player(player), myTurn(myTurn), tt() {
	board.assertValid(*cards, !myTurn);
}



void Game::waitTurn(Connection& connection) {
	auto result = connection.waitTurn(myTurn != player);

	U8 cardI = (U8)-1;
	for (U8 _cardI = 0; _cardI < 30; _cardI++) {
		auto& perm = CARDS_PERMUTATIONS[_cardI];
		if (((result.cardStr[0 + 2 * player] == std::string(cards->cards[perm.playerCards[0][0]].name) && result.cardStr[1 + 2 * player] == std::string(cards->cards[perm.playerCards[0][1]].name)) ||
		     (result.cardStr[1 + 2 * player] == std::string(cards->cards[perm.playerCards[0][0]].name) && result.cardStr[0 + 2 * player] == std::string(cards->cards[perm.playerCards[0][1]].name))) &&
		    ((result.cardStr[0 + 2 * !player] == std::string(cards->cards[perm.playerCards[1][0]].name) && result.cardStr[1 + 2 * !player] == std::string(cards->cards[perm.playerCards[1][1]].name)) ||
		     (result.cardStr[1 + 2 * !player] == std::string(cards->cards[perm.playerCards[1][0]].name) && result.cardStr[0 + 2 * !player] == std::string(cards->cards[perm.playerCards[1][1]].name))) &&
		     (result.cardStr[4] == std::string(cards->cards[perm.sideCard].name)))
			cardI = _cardI;
	}
	assert(cardI != (U8)-1);

	auto& perm = CARDS_PERMUTATIONS[cardI];

	ended = result.ended;
	myTurn = player != result.redTurn;
	board = parseBoard(result.boardStr, !player, !myTurn, cardI);
	board.checkValid(*cards, !myTurn, ended);
}



void Game::submitMove(Connection& connection, Board& nextBoard) {
	connection.submitMove(cards->cards, board, nextBoard, 0, !player);
}