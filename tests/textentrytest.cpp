#include "cppmarkdown/textentry.h"
#include "cppmarkdown/document.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Paragraph text entry", "[textentry]")
{
	Markdown::TextEntry te("regular text");
	REQUIRE(te.getText() == "regular text");
	REQUIRE(te.getHtml() == "regular text");
}

TEST_CASE("Italic text entry", "[textentry]")
{
	Markdown::TextEntry te("*italic*");
	REQUIRE(te.getText() == "italic");
	REQUIRE(te.getHtml() == "<i>italic</i>");
}

TEST_CASE("Bold text entry", "[textentry]")
{
	Markdown::TextEntry te("**bold**");
	REQUIRE(te.getText() == "bold");
	REQUIRE(te.getHtml() == "<b>bold</b>");
}

TEST_CASE("Bold italic entry", "[textentry]")
{
	Markdown::TextEntry te("**_bold_**");
	REQUIRE(te.getText() == "bold");
	REQUIRE(te.getHtml() == "<b><i>bold</i></b>");
}

TEST_CASE("Mixed bold styles", "[textentry]")
{
	Markdown::TextEntry te("**__bold__**");
	REQUIRE(te.getText() == "bold");
	REQUIRE(te.getHtml() == "<b>bold</b>");
}

TEST_CASE("Mixed nested styles", "[textentry]")
{
	{
		Markdown::TextEntry te("**bold _italic_**");
		REQUIRE(te.getText() == "bold italic");
		REQUIRE(te.getHtml() == "<b>bold <i>italic</i></b>");
	}

	{
		Markdown::TextEntry te("**some bold text _and italic_ and bold again**");
		REQUIRE(te.getText() == "some bold text and italic and bold again");
		REQUIRE(te.getHtml() == "<b>some bold text <i>and italic</i> and bold again</b>");
	}
}

TEST_CASE("Mixed italic and bold text entry", "[textentry]")
{
	Markdown::TextEntry te("a bit of *italic* and some **bold** end");
	REQUIRE(te.getText() == "a bit of italic and some bold end");
	REQUIRE(te.getHtml() == "a bit of <i>italic</i> and some <b>bold</b> end");
}

TEST_CASE("Nested italic and bold text entry", "[textentry]")
{
	Markdown::TextEntry te("some mixed *italic with **bold** end*");
	REQUIRE(te.getText() == "some mixed italic with bold end");
	REQUIRE(te.getHtml() == "some mixed <i>italic with <b>bold</b> end</i>");
}

TEST_CASE("Default styling", "[textentry]")
{
	Markdown::TextEntry te("italic by default", std::make_shared<Markdown::GenericStyle>("*", "*", "<i>", "</i>"));
	REQUIRE(te.getHtml() == "<i>italic by default</i>");
}

TEST_CASE("Links", "[textentry]")
{
	Markdown::TextEntry te("[Text](link)");
	REQUIRE(te.getHtml() == "<a href=\"link\">Text</a>");
}

TEST_CASE("Links with escapes", "[textentry]")
{
	Markdown::TextEntry te("[Text](link with (parentheses\\) and \\\"quotes\\\")");
	REQUIRE(te.getHtml() == "<a href=\"link with (parentheses) and &quot;quotes&quot;\">Text</a>");
}

TEST_CASE("Mixed links", "[textentry]")
{
	Markdown::TextEntry te("This is some [Text](link) blabla");
	REQUIRE(te.getHtml() == "This is some <a href=\"link\">Text</a> blabla");
}

TEST_CASE("Links with styles", "[textentry]")
{
	Markdown::TextEntry te("[**Bold** text](link)");
	REQUIRE(te.getHtml() == "<a href=\"link\"><b>Bold</b> text</a>");
}

TEST_CASE("Links with nested styles", "[textentry]")
{
	Markdown::TextEntry te("[**Bold _italic_** text](link)");
	REQUIRE(te.getHtml() == "<a href=\"link\"><b>Bold <i>italic</i></b> text</a>");
}

TEST_CASE("Inline code", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("blabla `some code` bla").getHtml() == "blabla <code>some code</code> bla");
}

TEST_CASE("Inline double code", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("blabla ``some code`` bla").getHtml() == "blabla <code>some code</code> bla");
	REQUIRE(Markdown::TextEntry("blabla ``some `bla` code`` bla").getHtml() == "blabla <code>some `bla` code</code> bla");
}

TEST_CASE("Images", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("![Alt](Link)").getHtml() == "<img src=\"Link\" alt=\"Alt\">");
}

TEST_CASE("Text entry in document", "[textentry]")
{
	Markdown::Document doc;
	doc.parse("Simple *italic* text");
	REQUIRE((*doc.begin())->getHtml() == "<p>Simple <i>italic</i> text</p>");
}

TEST_CASE("Italic and link in single TextEntry", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("Some text *italic* and [Some link](link)").getHtml() == "Some text <i>italic</i> and <a href=\"link\">Some link</a>");
}

TEST_CASE("Italic and image in single TextEntry", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("Some text *italic* and ![Some image](Link)").getHtml() == "Some text <i>italic</i> and <img src=\"Link\" alt=\"Some image\">");
}