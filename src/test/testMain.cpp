#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "testBoard.h"

#include <cstdlib>



void testMain() {
    doctest::Context context;

    int res = context.run();

    if(context.shouldExit())
        std::exit(1);
}