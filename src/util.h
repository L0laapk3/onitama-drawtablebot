#pragma once

#include "types.h"

#include <cassert>
#include <utility>



void inline assume(bool b) {
	assert(b);
	if (!b)
		std::unreachable();
}

constexpr U64 fact(int from, int downto = 0) {
	return from <= downto ? 1 : from * fact(from - 1, downto);
}