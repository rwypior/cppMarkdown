#include "textentry.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Paragraph text entry", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("regular text").getText() == "regular text");
	REQUIRE(Markdown::TextEntry("regular text").getHtml() == "regular text");
}

TEST_CASE("Italic text entry", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("*italic*").getText() == "italic");
	REQUIRE(Markdown::TextEntry("*italic*").getHtml() == "<i>italic</i>");
}

TEST_CASE("Bold text entry", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("**bold**").getText() == "bold");
	REQUIRE(Markdown::TextEntry("**bold**").getHtml() == "<b>bold</b>");
}

TEST_CASE("Mixed italic and bold text entry", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("a bit of *italic* and some **bold** end").getText() == "a bit of italic and some bold end");
	REQUIRE(Markdown::TextEntry("a bit of *italic* and some **bold** end").getHtml() == "a bit of <i>italic</i> and some <b>bold</b> end");
}

TEST_CASE("Nested italic and bold text entry", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("some mixed *italic with **bold** end*").getText() == "some mixed italic with bold end");
	REQUIRE(Markdown::TextEntry("some mixed *italic with **bold** end*").getHtml() == "some mixed <i>italic with <b>bold</b> end</i>");
}

TEST_CASE("Default styling", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("italic by default", std::make_shared<Markdown::GenericStyle>("*", "*", "<i>", "</i>")).getHtml() == "<i>italic by default</i>");
}

TEST_CASE("Links", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("[Text](link)").getHtml() == "<a href=\"link\">Text</a>");
}

TEST_CASE("Mixed links", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("This is some [Text](link) blabla").getHtml() == "This is some <a href=\"link\">Text</a> blabla");
}

TEST_CASE("Links with styles", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("[**Bold** text](link)").getHtml() == "<a href=\"link\"><b>Text</b></a>");
}

TEST_CASE("Inline code", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("blabla `some code` bla").getHtml() == "blabla <code>some code</code> bla");
	REQUIRE(Markdown::TextEntry("blabla ``some code`` bla").getHtml() == "blabla <code>some code</code> bla");
	REQUIRE(Markdown::TextEntry("blabla ``some `bla` code`` bla").getHtml() == "blabla <code>some `bla` code</code> bla");
}