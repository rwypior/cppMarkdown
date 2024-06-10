#include "cppmarkdown.h"

#include <catch/catch.hpp>

TEST_CASE("Parsing standard heading", "[heading]")
{
	Markdown::HeadingElement head(Markdown::HeadingElement::Heading::Heading1, "Heading!");
	
	REQUIRE(head.getText() == "Heading!");
	REQUIRE(head.getHtml() == "<h1>Heading!</h1>");
}