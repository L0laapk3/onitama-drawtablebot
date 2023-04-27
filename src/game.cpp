#include "game.h"



// leak some memory, whatever
Game::Game(std::array<std::string, 5> cardNames, std::string boardString, bool player, bool flipped) :
	Game(new CardsInfo(Connection::parseCards(cardNames, flipped)), Connection::parseBoard(boardString, flipped, player), player) { }

Game::Game(Connection& connection) : Game(connection.load()) { }

Game::Game(const Connection::LoadResult loadResult) : Game(new CardsInfo(loadResult.cards), loadResult.board, loadResult.myTurn,loadResult.player) { }

Game::Game(const CardsInfo* cards, const Board& board, bool player, bool myTurn) : cards(cards), board(board), myTurn(myTurn), tt() {

}