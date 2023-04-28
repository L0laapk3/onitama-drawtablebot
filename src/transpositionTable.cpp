#include "transpositionTable.h"



constexpr U64 SIZE_REPLACE_ALWAYS  = (1ULL << 20) / sizeof(Transposition); // 16MB, half of L3
constexpr U64 SIZE_DEPTH_PREFERRED = (1ULL << 20) / sizeof(Transposition); // 4GB


TranspositionTableWrapper::TranspositionTableWrapper() {
	TableReplaceAlways.resize(SIZE_REPLACE_ALWAYS);
	// TableDepthPreferred.resize(SIZE_DEPTH_PREFERRED);
}

Transposition TranspositionTableWrapper::get(U64 hash) const {
	if (TableReplaceAlways [hash & (SIZE_REPLACE_ALWAYS  - 1)].hash == hash) return TableReplaceAlways [hash & (SIZE_REPLACE_ALWAYS  - 1)];
	// if (TableDepthPreferred[hash & (SIZE_DEPTH_PREFERRED - 1)].hash == hash) return TableDepthPreferred[hash & (SIZE_DEPTH_PREFERRED - 1)];
	Transposition blank;
	blank.hash = 0;
	return blank;
}

void TranspositionTableWrapper::put(const Transposition trans) {
	auto& replaceAlwaysEntry = TableReplaceAlways[trans.hash & (SIZE_REPLACE_ALWAYS  - 1)];
	// if (replaceAlwaysEntry.hash) { // if theres something already in the replaceAlwaysTable, bump it down to the depthPreferredTable if the depth is greater
	// 	auto& depthPreferredEntry = TableDepthPreferred[trans.hash & (SIZE_DEPTH_PREFERRED - 1)];
	// 	if (depthPreferredEntry.depth < replaceAlwaysEntry.depth)
	// 		depthPreferredEntry = replaceAlwaysEntry;
	// }
	replaceAlwaysEntry = trans;
}