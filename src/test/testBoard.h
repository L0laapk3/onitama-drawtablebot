#pragma once

#include "doctest.h"
#include "board.h"
#include "boardIter.hpp"

#include <queue>
#include <sstream>
#include <algorithm>


// struct TestBoard : public Board {
// 	// contains all the meta info normally not stored in the board
// 	auto game = Game(&CARDS_PERFT);
// 	bool player = 0;

// 	static TestBoard create(const CardsInfo& cards, std::array<U32, 2> p = { 0b00000'00000'00000'00000'11111, 0b11111'00000'00000'00000'00000 }, std::array<U32, 2> k = { 0b00000'00000'00000'00000'00100, 0b00100'00000'00000'00000'00000 }, bool player = 0) {
// 		TestBoard board;
// 		(Board&)board = Board::create(player, p, k);
// 		board.cards = &cards;
// 		board.player = player;
// 		return board;
// 	}

// 	TestBoard invert() {
// 		TestBoard board;
// 		(Board&)board = Board::invert(board.player);
// 		board.cards = cards;
// 		board.player = !player;
// 		return board;
// 	}
// 	TestBoard invert(bool player) {
// 		assert(player == this->player);
// 		return invert();
// 	}

// 	template<bool player, typename Callable>
// 	void iterateMoves(const CardsInfo& cards, bool quiesence, const Callable f) {
// 		CHECK_EQ(player, this->player);
// 		this->player = !this->player;
// 		Board::iterateMoves<player>(cards, quiesence, f);
// 		this->player = !this->player;
// 	}
// };
// std::ostream& operator<<(std::ostream& os, const TestBoard& board) {
// 	os << board.toString(*board.cards, board.player);
// 	return os;
// }



// TEST_CASE("board.invert symmetry") {
// 	auto board = TestBoard::create(CARDS_PERFT), board2 = board;
// 	CHECK_EQ(board.invert().invert(), board2);
// }



// TEST_CASE("board.iter symmetry") {
// 	auto board = TestBoard::create(CARDS_PERFT), board2 = board;
// 	std::vector<TestBoard> childBoards;

// 	board.iterateMoves<0>(*board.cards, 0, [&]() {
// 		CHECK_NE(board, board2);
// 		childBoards.push_back(board.invert());
// 		INFO("generated", childBoards.back());
// 		return true;
// 	});

// 	board = board.invert();
// 	board.iterateMoves<1>(*board.cards, 0, [&]() {
// 		auto it = std::find(childBoards.begin(), childBoards.end(), board);
// 		REQUIRE_MESSAGE(it != childBoards.end(), "could not find", board);
// 		childBoards.erase(it);
// 		return true;
// 	});

// 	CHECK_EQ(childBoards.size(), 0);
// }



// TEST_CASE("move while losing")