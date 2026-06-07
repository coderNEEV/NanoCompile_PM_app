#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "broadcast.h"

TEST_CASE("Equal ranks")
{
    CHECK(
        broadcastShape({2, 3}, {2, 3}) ==
        std::vector<int64_t>{2, 3});
}

TEST_CASE("Different ranks")
{
    CHECK(
        broadcastShape({1, 32, 122, 122}, {32, 1, 1}) ==
        std::vector<int64_t>{1, 32, 122, 122});
}

TEST_CASE("Scalar broadcasting")
{
    CHECK(
        broadcastShape({}, {3, 4}) ==
        std::vector<int64_t>{3, 4});
}

TEST_CASE("Invalid mismatched dimensions")
{
    CHECK(
        !broadcastShape({2, 3}, {2, 4}).has_value());
}

TEST_CASE("High dimensional shapes")
{
    CHECK(
        broadcastShape(
            {1, 3, 1, 224},
            {4, 1, 5, 224})
        ==
        std::vector<int64_t>{4, 3, 5, 224});
}

TEST_CASE("Wrong side padding")
{
    CHECK(
        !broadcastShape({6, 8}, {6}).has_value());
}

TEST_CASE("Invalid dimension size")
{
    CHECK(
        !broadcastShape({2, 0, 4}, {2, 1, 4}).has_value());
        CHECK(
        !broadcastShape({2, 0, 4}, {2, -1, 4}).has_value());
}