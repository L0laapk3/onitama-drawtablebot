#include "transpositionTable.h"



TranspositionTableWrapper::TranspositionTableWrapper() {
	TableReplaceAlways.resize(TT_SIZE_REPLACE_ALWAYS);
	TableDepthPreferred.resize(TT_SIZE_DEPTH_PREFERRED);
}