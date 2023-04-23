#pragma once

#include "easywsclient.hpp"
#include <memory>
#include <array>

#include "board.h"
#include "card.hpp"



class Game;
class Connection {
public:
	Connection();
	Connection(std::string matchId);
	~Connection();

	static CardsInfo parseCards(std::array<std::string, 5> cardNames, bool flipped);
	static Board parseBoard(std::string str, bool flip);

	void sendCreate();
	void sendJoin(const std::string& matchId);

	struct LoadResult {
		bool myTurn;
		CardsInfo cards;
		Board board;
	};
	LoadResult load();
	void waitTurn(Game& game);
	void submitMove(Game& game, const Board& board);

	std::string matchId;
	std::string token;
	std::string opponent;

	int index;
	bool player;
	bool ended = false;

private:
	void handleJoinGame();
	std::unique_ptr<easywsclient::WebSocket> ws = nullptr;
};