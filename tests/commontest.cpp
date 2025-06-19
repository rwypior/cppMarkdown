#include "cppmarkdown/cppmarkdowncommon.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Split", "[common]")
{
    REQUIRE(Markdown::split("1|2|3", '|') == std::vector<std::string>{"1", "2", "3"});
    REQUIRE(Markdown::split("1|2|", '|') == std::vector<std::string>{"1", "2", ""});
}

TEST_CASE("Escaping", "[common]")
{
    REQUIRE(Markdown::isEscaped("Something", 3) == false);
    REQUIRE(Markdown::isEscaped("Som\\ething", 4) == true);
    REQUIRE(Markdown::isEscaped("Som\\\\ething", 5) == false);
    REQUIRE(Markdown::isEscaped("Som\\\\ething", 4) == true);
    REQUIRE(Markdown::isEscaped("Som\\\\\\ething", 6) == true);
    REQUIRE(Markdown::isEscaped("Som\\\\\\ething", 0) == false);
    REQUIRE(Markdown::isEscaped("Som\\\\\\ething", 8) == false);
    REQUIRE(Markdown::isEscaped("", 3) == false);
}