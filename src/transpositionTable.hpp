#include "transpositionTable.h"



inline bool TranspositionTableWrapper::get(U64 hash, const Transposition*& result) const {
	auto& entry = table[hash & (TT_SIZE - 1)];
	result = &entry.depthPreferred;
	if (result->hash != hash) // if depthPreferred doesn't match, try replaceAlways
		result++;
	return result->hash == hash;
}

inline void TranspositionTableWrapper::put(const Transposition trans) {
	auto& entry = table[trans.hash & (TT_SIZE  - 1)];

	// write to depthPreferred if it can, otherwise to replaceAlways
    Transposition* writeEntry = &entry.depthPreferred;
	if (entry.depthPreferred.depth >= trans.depth)
        writeEntry++;
    *writeEntry = trans;
}