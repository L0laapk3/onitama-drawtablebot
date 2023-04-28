#define NOMINMAX

#include "connection.h"
#include "game.h"

#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <regex>
#include <iostream>
#include "shellapi.h"

#include <windows.h>
#include <shellapi.h>

#include <immintrin.h>
#include <x86intrin.h>



std::string getRegex(const std::string& message, const std::string reg) {
	std::smatch match;
	if (std::regex_search(message, match, std::regex(reg)) && match.size() > 1)
		return match.str(1);
	return "";
}
std::string getString(const std::string& message, const std::string key) {
	return getRegex(message, "\"" + key + "\":\"([^\"]+)\"");
}
int getInt(const std::string& message, const std::string key) {
	std::string result = getRegex(message, "\"" + key + "\":(\\d+)");
	return result.size() ? std::stoi(result) : -1;
}


Connection::Connection() {
#ifdef _WIN32
	INT rc;
	WSADATA wsaData;

	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rc) {
		printf("WSAStartup Failed.\n");
	}
#endif
	ws = std::unique_ptr<easywsclient::WebSocket>(easywsclient::WebSocket::from_url("ws://server.lucasholten.com/onitama"));
	assert(ws);
	assert(ws->getReadyState() != easywsclient::WebSocket::CLOSED);
}

Connection::~Connection() {
#ifdef _WIN32
	WSACleanup();
#endif
}


void Connection::handleJoinGame() {
	while (!matchId.size()) {
		ws->poll(-1);
		assert(ws->getReadyState() != easywsclient::WebSocket::CLOSED);
		ws->dispatch([&](const std::string& message) {
			matchId = getString(message, "matchId");
			token = getString(message, "token");
			index = getInt(message, "index");
		});
	}
	assert(token.size());
	assert(index != -1);

	ws->send("spectate " + matchId);

	std::string webUrl = "https://l0laapk3.github.io/Onitama-client/#spectate-" + matchId;
	ShellExecuteA(NULL, "open", webUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
	std::cout << "https://git.io/onitama#spectate-" << matchId << std::endl;
}

void Connection::sendCreate() {
	ws->send("create Botama");
	handleJoinGame();
}
void Connection::sendJoin(const std::string& matchId) {
	ws->send("join " + matchId + " Botama");
	handleJoinGame();
}



Board Connection::parseBoard(std::string str, bool flip, bool player, U8 cardI) {
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

CardsInfo Connection::parseCards(std::array<std::string, 5> cardNames, bool flipped) {
    return CardsInfo::create({
        findCard(cardNames[flipped ? 2 : 0]),
        findCard(cardNames[flipped ? 3 : 1]),
        findCard(cardNames[flipped ? 0 : 2]),
        findCard(cardNames[flipped ? 1 : 3]),
        findCard(cardNames[4]),
    });
}

Connection::LoadResult Connection::load() {
	assert(ws->getReadyState() != easywsclient::WebSocket::CLOSED);

	std::string boardStr = "";
	while (true) {
		ws->poll(-1);
		assert(ws->getReadyState() != easywsclient::WebSocket::CLOSED);
		std::array<std::string, 5> cards;
		bool myTurn, player;
		ws->dispatch([&](const std::string& message) {
			boardStr = getString(message, "board");
			if (boardStr.size()) {
				player = std::stoi(getRegex(message, "\"indices\":[^}]+\"red\":(\\d+)")) == index;
				cards[0] = getRegex(message, "\"cards\":[^}]+\"blue\":\\[\"([^\"]+)\"");
				cards[1] = getRegex(message, "\"cards\":[^}]+\"blue\":\\[\"[^\"]+\",\"([^\"]+)\"");
				cards[2] = getRegex(message, "\"cards\":[^}]+\"red\":\\[\"([^\"]+)\"");
				cards[3] = getRegex(message, "\"cards\":[^}]+\"red\":\\[\"[^\"]+\",\"([^\"]+)\"");
				cards[4] = getRegex(message, "\"cards\":[^}]+\"side\":\"([^\"]+)\"");
				myTurn = getString(message, "currentTurn") == (player ? "red" : "blue");
			}
		});
		if (boardStr.size()) {
			Board board = parseBoard(boardStr, player, !myTurn);
			auto cardsInfo = parseCards(cards, player);
			board.checkValid(cardsInfo, !myTurn);
			return {
				.player = player,
				.myTurn = myTurn,
				.cards = cardsInfo,
				.board = board,
			};
		}
	}
}

void Connection::waitTurn(Game& game) {
	std::string boardStr = "";
	bool lastTurn = game.myTurn;
	std::array<std::string, 5> cards;
	while (!boardStr.size() || (lastTurn == game.myTurn)) {
		ws->poll(-1);
		assert(ws->getReadyState() != easywsclient::WebSocket::CLOSED);
		ws->dispatch([&](const std::string& message) {
			boardStr = getString(message, "board");
			cards[0] = getRegex(message, "\"cards\":[^}]+\"blue\":\\[\"([^\"]+)\"");
			cards[1] = getRegex(message, "\"cards\":[^}]+\"blue\":\\[\"[^\"]+\",\"([^\"]+)\"");
			cards[2] = getRegex(message, "\"cards\":[^}]+\"red\":\\[\"([^\"]+)\"");
			cards[3] = getRegex(message, "\"cards\":[^}]+\"red\":\\[\"[^\"]+\",\"([^\"]+)\"");
			cards[4] = getRegex(message, "\"cards\":[^}]+\"side\":\"([^\"]+)\"");
			game.myTurn = getString(message, "currentTurn") == (game.player ? "red" : "blue");
			game.ended = getString(message, "gameState") == "ended";
		});
	}

	U8 cardI = (U8)-1;
	for (U8 _cardI = 0; _cardI < 30; _cardI++) {
		auto& perm = CARDS_PERMUTATIONS[_cardI];
		if (((cards[0 + 2 * game.player] == std::string(game.cards->cards[perm.playerCards[0][0]].name) && cards[1 + 2 * game.player] == std::string(game.cards->cards[perm.playerCards[0][1]].name)) ||
		     (cards[1 + 2 * game.player] == std::string(game.cards->cards[perm.playerCards[0][0]].name) && cards[0 + 2 * game.player] == std::string(game.cards->cards[perm.playerCards[0][1]].name))) &&
		    ((cards[0 + 2 * !game.player] == std::string(game.cards->cards[perm.playerCards[1][0]].name) && cards[1 + 2 * !game.player] == std::string(game.cards->cards[perm.playerCards[1][1]].name)) ||
		     (cards[1 + 2 * !game.player] == std::string(game.cards->cards[perm.playerCards[1][0]].name) && cards[0 + 2 * !game.player] == std::string(game.cards->cards[perm.playerCards[1][1]].name))) &&
		     (cards[4] == std::string(game.cards->cards[perm.sideCard].name)))
			cardI = _cardI;
	}
	assert(cardI != (U8)-1);

	auto& perm = CARDS_PERMUTATIONS[cardI];

	game.board = parseBoard(boardStr, game.player, !game.myTurn, cardI);

	game.board.recalculateHash(!game.myTurn);
	game.board.checkValid(*game.cards, !game.myTurn, game.ended);
}

std::string indexToPos(U32 i, bool flipped) {
	assert(i < 25);
	return (flipped ? "edcba" : "abcde")[i % 5] + std::to_string((flipped ? 24 - i : i) / 5 + 1);
}

void Connection::submitMove(Game& game, const Board& board, bool flipped) {
	U32 from = _tzcnt_u32(game.board.p[flipped] & ~board.p[flipped]);
	U32 to   = _tzcnt_u32(board.p[flipped] & ~game.board.p[flipped]);
	auto permutation = CARDS_PERMUTATIONS[board.cardI];
	const Card& card = game.cards->cards[permutation.sideCard];
	const std::string moveStr = std::string(card.name) + ' ' + indexToPos(from, game.player != flipped) + indexToPos(to, game.player != flipped);
	// std::cout << "move " << moveStr << std::endl;
	ws->send("move " + matchId + ' ' + token + ' ' + moveStr);
}
