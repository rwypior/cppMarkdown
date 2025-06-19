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
	REQUIRE(te.getHtml() == "<em>italic</em>");
}

TEST_CASE("Bold text entry", "[textentry]")
{
	Markdown::TextEntry te("**bold**");
	REQUIRE(te.getText() == "bold");
	REQUIRE(te.getHtml() == "<strong>bold</strong>");
}

TEST_CASE("Bold italic entry", "[textentry]")
{
	Markdown::TextEntry te("**_bold_**");
	REQUIRE(te.getText() == "bold");
	REQUIRE(te.getHtml() == "<strong><em>bold</em></strong>");
}

TEST_CASE("Mixed bold styles", "[textentry]")
{
	Markdown::TextEntry te("**__bold__**");
	REQUIRE(te.getText() == "bold");
	REQUIRE(te.getHtml() == "<strong>bold</strong>");
}

TEST_CASE("Mixed nested styles", "[textentry]")
{
	{
		Markdown::TextEntry te("**bold _italic_**");
		REQUIRE(te.getText() == "bold italic");
		REQUIRE(te.getHtml() == "<strong>bold <em>italic</em></strong>");
	}

	{
		Markdown::TextEntry te("**some bold text _and italic_ and bold again**");
		REQUIRE(te.getText() == "some bold text and italic and bold again");
		REQUIRE(te.getHtml() == "<strong>some bold text <em>and italic</em> and bold again</strong>");
	}
}

TEST_CASE("Mixed italic and bold text entry", "[textentry]")
{
	Markdown::TextEntry te("a bit of *italic* and some **bold** end");
	REQUIRE(te.getText() == "a bit of italic and some bold end");
	REQUIRE(te.getHtml() == "a bit of <em>italic</em> and some <strong>bold</strong> end");
}

TEST_CASE("Nested italic and bold text entry", "[textentry]")
{
	Markdown::TextEntry te("some mixed *italic with **bold** end*");
	REQUIRE(te.getText() == "some mixed italic with bold end");
	REQUIRE(te.getHtml() == "some mixed <em>italic with <strong>bold</strong> end</em>");
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

TEST_CASE("Imcomplete link", "[textentry]")
{
	{
		Markdown::TextEntry te("[Text blabla");
		REQUIRE(te.getHtml() == "[Text blabla");
	}
	
	{
		Markdown::TextEntry te("[Text](blabla");
		REQUIRE(te.getHtml() == "[Text](blabla");
	}
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
	REQUIRE(te.getHtml() == "<a href=\"link\"><strong>Bold</strong> text</a>");
}

TEST_CASE("Links with nested styles", "[textentry]")
{
	Markdown::TextEntry te("[**Bold _italic_** text](link)");
	REQUIRE(te.getHtml() == "<a href=\"link\"><strong>Bold <em>italic</em></strong> text</a>");
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

TEST_CASE("Imcomplete images", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("!Something").getHtml() == "!Something");
	REQUIRE(Markdown::TextEntry("![alt").getHtml() == "![alt");
	REQUIRE(Markdown::TextEntry("![alt](blabla").getHtml() == "![alt](blabla");
}

TEST_CASE("Text entry in document", "[textentry]")
{
	Markdown::Document doc;
	doc.parse("Simple *italic* text");
	REQUIRE((*doc.begin())->getHtml() == "<p>Simple <em>italic</em> text</p>");
}

TEST_CASE("Italic and link in single TextEntry", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("Some text *italic* and [Some link](link)").getHtml() == "Some text <em>italic</em> and <a href=\"link\">Some link</a>");
}

TEST_CASE("Italic and image in single TextEntry", "[textentry]")
{
	REQUIRE(Markdown::TextEntry("Some text *italic* and ![Some image](Link)").getHtml() == "Some text <em>italic</em> and <img src=\"Link\" alt=\"Some image\">");
}