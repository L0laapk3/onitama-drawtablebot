#include "transpositionTable.h"



inline Transposition TranspositionTableWrapper::get(U64 hash) const {
	auto& entry = table[hash & (TT_SIZE - 1)];
	if (entry.depthPreferred.hash == hash) return entry.depthPreferred;
	if (entry.replaceAlways.hash  == hash) return entry.replaceAlways;
	Transposition blank;
	blank.hash = 0;
	return blank;
}

inline void TranspositionTableWrapper::put(const Transposition trans) {
	auto& entry = table[trans.hash & (TT_SIZE  - 1)];

	// code is the same, but the compiler does not see the optimization

	// if (entry.depthPreferred.depth < trans.depth)
    //     entry.depthPreferred = trans;
    // else
    //     entry.replaceAlways = trans;

    Transposition* writeEntry = &entry.depthPreferred;
	if (entry.depthPreferred.depth >= trans.depth)
        writeEntry++;
    *writeEntry = trans;
}