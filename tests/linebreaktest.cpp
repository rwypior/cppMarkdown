#include "cppmarkdown/linebreakelement.h"
#include "cppmarkdown/document.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Line break skippable", "[linebreak]")
{
	REQUIRE(Markdown::LineBreakElement::isSkippable("") == true);
	REQUIRE(Markdown::LineBreakElement::isSkippable(" ") == true);
	REQUIRE(Markdown::LineBreakElement::isSkippable("  ") == false);
	REQUIRE(Markdown::LineBreakElement::isSkippable("   ") == false);
}

TEST_CASE("Line break all spaces", "[linebreak]")
{
	REQUIRE(Markdown::LineBreakElement::isAllWhitespace(" ") == true);
	REQUIRE(Markdown::LineBreakElement::isAllWhitespace("\t") == true);
	REQUIRE(Markdown::LineBreakElement::isAllWhitespace("") == true);
	REQUIRE(Markdown::LineBreakElement::isAllWhitespace(" x") == false);
	REQUIRE(Markdown::LineBreakElement::isAllWhitespace(" x ") == false);
	REQUIRE(Markdown::LineBreakElement::isAllWhitespace("x ") == false);
	REQUIRE(Markdown::LineBreakElement::isAllWhitespace("x") == false);
}

TEST_CASE("Line break between paragraphs", "[document]")
{
	std::string markdown = R"md(First paragraph

Second paragraph)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<p>First paragraph</p>"
		"<br>"
		"<p>Second paragraph</p>"
		"</body></html>"
	);
}

TEST_CASE("Line break after title", "[document]")
{
	std::string markdown = R"md(Title
=====

Second paragraph)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<h1>Title</h1>"
		"<br>"
		"<p>Second paragraph</p>"
		"</body></html>"
	);
}

TEST_CASE("Line break before list", "[document]")
{
	std::string markdown = R"md(Title
=====

- a
- b
- c)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<h1>Title</h1>"
		"<br>"
		"<ul>"
		"<li>a</li>"
		"<li>b</li>"
		"<li>c</li>"
		"</ul>"
		"</body></html>"
	);
}

TEST_CASE("Multiple line breaks", "[document]")
{
	std::string markdown = R"md(Para 1
Para 2

Para 3



Para 4)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<p>Para 1</p>"
		"<p>Para 2</p>"
		"<br>"
		"<p>Para 3</p>"
		"<br>"
		"<p>Para 4</p>"
		"</body></html>"
	);
}