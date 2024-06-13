#include "listelement.h"

#include <catch2/catch.hpp>

TEST_CASE("Simple list", "[list]")
{
	Markdown::ListElement el(R"list(1. first
2. second
3. third)list");
	REQUIRE(el.getText() == R"list(1. first
2. second
3. third)list");
	REQUIRE(el.getHtml() == R"list(<ol><li>First item</li><li>Second item</li><li>Third item</li></ol>)list");
}