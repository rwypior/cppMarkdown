#include "cppmarkdown.h"

#include <catch/catch.hpp>

TEST_CASE("Html conversion", "[textentry]")
{
	//REQUIRE(Markdown::TextEntry("regular text").getHtml() == "regular text");
	REQUIRE(Markdown::TextEntry("*italic*").getHtml() == "<i>italic</i>");
	REQUIRE(Markdown::TextEntry("**bold**").getHtml() == "<b>bold</b>");
	REQUIRE(Markdown::TextEntry("a bit of *italic* and some **bold**").getHtml() == "a bit of <i>italic</i> and some <b>bold</b>");
	REQUIRE(Markdown::TextEntry("some mixed *italic with **bold** end*").getHtml() == "some mixed <i>italic with <b>bold</b> end</i>");
}