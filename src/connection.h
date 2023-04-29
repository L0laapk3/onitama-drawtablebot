#pragma once

#include "easywsclient.hpp"
#include <memory>
#include <array>
#include <string>

#include "board.h"
#include "card.hpp"



class Game;
class Connection {
public:
	Connection();
	Connection(std::string matchId);
	~Connection();

	void sendCreate();
	void sendJoin(const std::string& matchId);

	struct LoadResult {
		bool playerBlue;
		bool redTurn;
		bool ended = false;
		std::array<std::string, 5> cardStr;
		std::string boardStr;
	};
	LoadResult load();
	LoadResult waitTurn(bool redTurn);
	void submitMove(const CardSet& cards, const Board& prevBoard, const Board& nextBoard, bool player, bool flipped);

	std::string matchId;
	std::string token;
	std::string opponent;

	int index;

private:
	void handleJoinGame();
	std::unique_ptr<easywsclient::WebSocket> ws = nullptr;
};