#include "transpositionTable.h"



inline Transposition TranspositionTableWrapper::get(U64 hash) const {
	auto& entry = table[hash & (TT_SIZE - 1)];
	if (entry.replaceAlways.hash == hash) return entry.replaceAlways;
	if (entry.depthPreferred.hash == hash) return entry.depthPreferred;
	Transposition blank;
	blank.hash = 0;
	return blank;
}

inline void TranspositionTableWrapper::put(const Transposition trans) {
	auto& entry = table[trans.hash & (TT_SIZE  - 1)];
	// this check is optional, not doing it is faster
	// if (entry.replaceAlways.hash)
		if (entry.depthPreferred.depth < entry.replaceAlways.depth)
			entry.depthPreferred = entry.replaceAlways;
	entry.replaceAlways = trans;
}