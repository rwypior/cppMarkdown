#include "textentry.h"

#include <catch2/catch.hpp>

TEST_CASE("Text conversion", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("regular text").getText() == "regular text");
	REQUIRE(Markdown::TextEntry("*italic*").getText() == "italic");
	REQUIRE(Markdown::TextEntry("**bold**").getText() == "bold");
	REQUIRE(Markdown::TextEntry("a bit of *italic* and some **bold** end").getText() == "a bit of italic and some bold end");
	REQUIRE(Markdown::TextEntry("some mixed *italic with **bold** end*").getText() == "some mixed italic with bold end");
}

TEST_CASE("Html conversion", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("regular text").getHtml() == "regular text");
	REQUIRE(Markdown::TextEntry("*italic*").getHtml() == "<i>italic</i>");
	REQUIRE(Markdown::TextEntry("**bold**").getHtml() == "<b>bold</b>");
	REQUIRE(Markdown::TextEntry("a bit of *italic* and some **bold** end").getHtml() == "a bit of <i>italic</i> and some <b>bold</b> end");
	REQUIRE(Markdown::TextEntry("some mixed *italic with **bold** end*").getHtml() == "some mixed <i>italic with <b>bold</b> end</i>");
}

TEST_CASE("Default styling", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("italic by default", Markdown::MarkdownStyle("*", "*", "<i>", "</i>")).getHtml() == "<i>italic by default</i>");
}

TEST_CASE("Links", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("[Text](link)").getHtml() == "<a href=\"link\">Text</a>");
}

TEST_CASE("Mixed links", "[textentry]")
{
	Markdown::TextEntry asda("This is some [Text](link) blabla");
	REQUIRE(Markdown::TextEntry("This is some [Text](link) blabla").getHtml() == "This is some <a href=\"link\">Text</a> blabla");
}