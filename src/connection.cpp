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



Connection::LoadResult Connection::load() {
	assert(ws->getReadyState() != easywsclient::WebSocket::CLOSED);

	std::string boardStr = "";
	while (true) {
		ws->poll(-1);
		assert(ws->getReadyState() != easywsclient::WebSocket::CLOSED);
		std::array<std::string, 5> cards;
		bool redTurn, playerBlue;
		ws->dispatch([&](const std::string& message) {
			boardStr = getString(message, "board");
			if (boardStr.size()) {
				playerBlue = std::stoi(getRegex(message, "\"indices\":[^}]+\"red\":(\\d+)")) != index;
				cards[0] = getRegex(message, "\"cards\":[^}]+\"red\":\\[\"([^\"]+)\"");
				cards[1] = getRegex(message, "\"cards\":[^}]+\"red\":\\[\"[^\"]+\",\"([^\"]+)\"");
				cards[2] = getRegex(message, "\"cards\":[^}]+\"blue\":\\[\"([^\"]+)\"");
				cards[3] = getRegex(message, "\"cards\":[^}]+\"blue\":\\[\"[^\"]+\",\"([^\"]+)\"");
				cards[4] = getRegex(message, "\"cards\":[^}]+\"side\":\"([^\"]+)\"");
				redTurn = getString(message, "currentTurn") == "red";
			}
		});
		if (boardStr.size()) {
			return {
				.playerBlue = playerBlue,
				.redTurn = redTurn,
				.cardStr = cards,
				.boardStr = boardStr,
			};
		}
	}
}

Connection::LoadResult Connection::waitTurn(bool redTurn) {
	std::string boardStr = "";
	bool newRedTurn = redTurn;
	bool ended, playerBlue;
	std::array<std::string, 5> cards;
	while (!boardStr.size() || (newRedTurn == redTurn)) {
		ws->poll(-1);
		assert(ws->getReadyState() != easywsclient::WebSocket::CLOSED);
		ws->dispatch([&](const std::string& message) {
			boardStr = getString(message, "board");
			cards[0] = getRegex(message, "\"cards\":[^}]+\"red\":\\[\"([^\"]+)\"");
			cards[1] = getRegex(message, "\"cards\":[^}]+\"red\":\\[\"[^\"]+\",\"([^\"]+)\"");
			cards[2] = getRegex(message, "\"cards\":[^}]+\"blue\":\\[\"([^\"]+)\"");
			cards[3] = getRegex(message, "\"cards\":[^}]+\"blue\":\\[\"[^\"]+\",\"([^\"]+)\"");
			cards[4] = getRegex(message, "\"cards\":[^}]+\"side\":\"([^\"]+)\"");
			newRedTurn = getString(message, "currentTurn") == "red";
			ended = getString(message, "gameState") == "ended";
		});
	}

	return {
		.redTurn = newRedTurn,
		.ended = ended,
		.cardStr = cards,
		.boardStr = boardStr,
	};
}

std::string indexToPos(U32 i, bool flipped) {
	assert(i < 25);
	return (flipped ? "edcba" : "abcde")[i % 5] + std::to_string((flipped ? 24 - i : i) / 5 + 1);
}

void Connection::submitMove(const CardSet& cards, const Board& prevBoard, const Board& nextBoard, bool player, bool flipped) {
	U32 from = _tzcnt_u32(prevBoard.p[player] & ~nextBoard.p[player]);
	U32 to   = _tzcnt_u32(nextBoard.p[player] & ~prevBoard.p[player]);
	auto permutation = CARDS_PERMUTATIONS[nextBoard.cardI];
	const Card& card = cards[permutation.sideCard];
	const std::string moveStr = std::string(card.name) + ' ' + indexToPos(from, player != flipped) + indexToPos(to, player != flipped);
	std::cout << "move " << moveStr << std::endl;
	ws->send("move " + matchId + ' ' + token + ' ' + moveStr);
}
