#pragma once

#include "doctest.h"
#include "board.h"
#include "boardIter.hpp"

#include <queue>
#include <sstream>


struct TestBoard : public Board {
	// contains all the meta info normally not stored in the board
	const CardsInfo* cards = &CARDS_PERFT;
	bool player = 0;

	static TestBoard create(const CardsInfo& cards, std::array<U32, 2> p = { 0b00000'00000'00000'00000'11111, 0b11111'00000'00000'00000'00000 }, std::array<U32, 2> k = { 0b00000'00000'00000'00000'00100, 0b00100'00000'00000'00000'00000 }, bool player = 0) {
		TestBoard board;
		(Board&)board = Board::create(p, k, player);
		board.cards = &cards;
		board.player = player;
		return board;
	}

	TestBoard invert() {
		TestBoard board;
		(Board&)board = Board::invert();
		board.cards = cards;
		board.player = !player;
		return board;
	}

	template<bool player, typename Callable>
	void iterateMoves(const CardsInfo& cards, bool quiesence, const Callable f) {
		CHECK_EQ(player, this->player);
		this->player = !this->player;
		Board::iterateMoves<player>(cards, quiesence, f);
		this->player = !this->player;
	}
};
std::ostream& operator<<(std::ostream& os, const TestBoard& board) {
	os << board.toString(*board.cards, board.player);
	return os;
}



TEST_CASE("board.invert symmetry") {
	TestBoard board = TestBoard::create(CARDS_PERFT), board2 = board;
	CHECK_EQ(board.invert().invert(), board2);
}



TEST_CASE("board.iter symmetry") {
	TestBoard board = TestBoard::create(CARDS_PERFT);
	std::queue<TestBoard> childBoards;

	board.iterateMoves<0>(*board.cards, 0, [&]() {
		childBoards.push(board);
		return true;
	});

	board.invert().iterateMoves<1>(*board.cards, 0, [&]() {
		CHECK_EQ(board, childBoards.front().invert());
		childBoards.pop();
		return true;
	});

	CHECK_EQ(childBoards.size(), 0);
}