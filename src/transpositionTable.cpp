#include "transpositionTable.h"



constexpr U64 SIZE_REPLACE_ALWAYS = 1ULL << 24; // 16MB, half of L3
constexpr U64 SIZE_DEPTH_PREFERRED = 1ULL << 32; // 4GB


TranspositionTableWrapper::TranspositionTableWrapper() {
	TableReplaceAlways.resize(SIZE_REPLACE_ALWAYS);
	TableDepthPreferred.resize(SIZE_DEPTH_PREFERRED);
}

Transposition TranspositionTableWrapper::get(U64 hash) {
	if (TableReplaceAlways [hash & (SIZE_REPLACE_ALWAYS  - 1)].hash == hash) return TableReplaceAlways [hash & (SIZE_REPLACE_ALWAYS  - 1)];
	if (TableDepthPreferred[hash & (SIZE_DEPTH_PREFERRED - 1)].hash == hash) return TableDepthPreferred[hash & (SIZE_DEPTH_PREFERRED - 1)];
	return Transposition{};
}

void TranspositionTableWrapper::put(U64 hash, Transposition& transposition) {
	auto& replaceAlwaysEntry = TableReplaceAlways[hash & (SIZE_REPLACE_ALWAYS  - 1)];
	if (replaceAlwaysEntry.hash) { // if theres something already in the replaceAlwaysTable, bump it down to the depthPreferredTable if the depth is greater
		auto& depthPreferredEntry = TableDepthPreferred[hash & (SIZE_DEPTH_PREFERRED - 1)];
		if (depthPreferredEntry.depth < replaceAlwaysEntry.depth)
			depthPreferredEntry = replaceAlwaysEntry;
	}
	replaceAlwaysEntry = transposition;
}