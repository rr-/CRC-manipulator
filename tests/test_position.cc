#include "catch.hh"
#include "util.h"

TEST_CASE("Automatic patch insertion position works", "[pos]")
{
    REQUIRE(computeAutoPosition(0, 4, false) == 0);
    REQUIRE(computeAutoPosition(0, 2, false) == 0);
    REQUIRE(computeAutoPosition(2, 4, false) == 2);
    REQUIRE(computeAutoPosition(2, 2, false) == 2);
}

TEST_CASE("Automatic patch overwrite position works", "[pos]")
{
    REQUIRE_THROWS(computeAutoPosition(0, 4, true));
    REQUIRE_THROWS(computeAutoPosition(2, 4, true));
    REQUIRE(computeAutoPosition(4, 4, true) == 0);
    REQUIRE(computeAutoPosition(5, 4, true) == 1);
}

TEST_CASE("Manual patch insertion position works", "[pos]")
{
    REQUIRE(shiftUserPosition(0, 4, 4, false) == 0);
    REQUIRE(shiftUserPosition(1, 4, 4, false) == 1);
    REQUIRE(shiftUserPosition(2, 4, 4, false) == 2);
    REQUIRE(shiftUserPosition(3, 4, 4, false) == 3);
    REQUIRE(shiftUserPosition(4, 4, 4, false) == 4);
    REQUIRE_THROWS(shiftUserPosition(5, 4, 4, false));
}

TEST_CASE("Manual patch overwrite position works", "[pos]")
{
    REQUIRE(shiftUserPosition(0, 4, 4, true) == 0);
    REQUIRE_THROWS(shiftUserPosition(1, 4, 4, true));
    REQUIRE_THROWS(shiftUserPosition(2, 4, 4, true));
    REQUIRE_THROWS(shiftUserPosition(3, 4, 4, true));
    REQUIRE_THROWS(shiftUserPosition(4, 4, 4, true));
    REQUIRE_THROWS(shiftUserPosition(5, 4, 4, true));
    REQUIRE(shiftUserPosition(0, 4, 2, true) == 0);
    REQUIRE(shiftUserPosition(1, 4, 2, true) == 1);
    REQUIRE(shiftUserPosition(2, 4, 2, true) == 2);
    REQUIRE_THROWS(shiftUserPosition(3, 4, 2, true));
}

TEST_CASE("Manual negative patch insertion position works", "[pos]")
{
    REQUIRE(shiftUserPosition(-0, 4, 4, false) == 0);
    REQUIRE(shiftUserPosition(-1, 4, 4, false) == 3);
    REQUIRE(shiftUserPosition(-2, 4, 4, false) == 2);
    REQUIRE(shiftUserPosition(-3, 4, 4, false) == 1);
    REQUIRE(shiftUserPosition(-4, 4, 4, false) == 0);
    REQUIRE(shiftUserPosition(-5, 4, 4, false) == 3);
}

TEST_CASE("Manual negative patch overwrite position works", "[pos]")
{
    REQUIRE(shiftUserPosition(-0, 4, 4, true) == 0);
    REQUIRE_THROWS(shiftUserPosition(-1, 4, 4, true));
    REQUIRE_THROWS(shiftUserPosition(-2, 4, 4, true));
    REQUIRE_THROWS(shiftUserPosition(-3, 4, 4, true));
    REQUIRE(shiftUserPosition(-4, 4, 4, true) == 0);
    REQUIRE_THROWS(shiftUserPosition(-5, 4, 4, true));
}
