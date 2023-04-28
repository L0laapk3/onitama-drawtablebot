#include "transpositionTable.h"



inline Transposition TranspositionTableWrapper::get(U64 hash) const {
	if (TableReplaceAlways [hash & (TT_SIZE_REPLACE_ALWAYS  - 1)].hash == hash) return TableReplaceAlways [hash & (TT_SIZE_REPLACE_ALWAYS  - 1)];
	if (TableDepthPreferred[hash & (TT_SIZE_DEPTH_PREFERRED - 1)].hash == hash) return TableDepthPreferred[hash & (TT_SIZE_DEPTH_PREFERRED - 1)];
	Transposition blank;
	blank.hash = 0;
	return blank;
}

inline void TranspositionTableWrapper::put(const Transposition trans) {
	auto& replaceAlwaysEntry = TableReplaceAlways[trans.hash & (TT_SIZE_REPLACE_ALWAYS  - 1)];
	if (replaceAlwaysEntry.hash) { // if theres something already in the replaceAlwaysTable, bump it down to the depthPreferredTable if the depth is greater
		auto& depthPreferredEntry = TableDepthPreferred[trans.hash & (TT_SIZE_DEPTH_PREFERRED - 1)];
		if (depthPreferredEntry.depth < replaceAlwaysEntry.depth)
			depthPreferredEntry = replaceAlwaysEntry;
	}
	replaceAlwaysEntry = trans;
}