#include "cppmarkdowncommon.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Split", "[common]")
{
    REQUIRE(Markdown::split("1|2|3", '|') == std::vector<std::string>{"1", "2", "3"});
    REQUIRE(Markdown::split("1|2|", '|') == std::vector<std::string>{"1", "2", ""});
}
