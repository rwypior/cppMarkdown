#include "blockquoteelement.h"

#include <catch2/catch.hpp>

TEST_CASE("Simple blockquote", "[blockquote]")
{
	Markdown::BlockquoteElement el("Im a blockquote!");
	REQUIRE(el.getText() == "Im a blockquote!");
	REQUIRE(el.getHtml() == "<blockquote><p>Im a blockquote!</p></blockquote>");
}

TEST_CASE("Multiline blockquote", "[blockquote]")
{
	Markdown::BlockquoteElement el(
		"Im a blockquote!\n"
		"With two lines!"
	);
	REQUIRE(el.getText() ==
		"Im a blockquote!\n"
		"With two lines!");
	REQUIRE(el.getHtml() ==
		"<blockquote><p>Im a blockquote!</p><p>With two lines!</p></blockquote>");
}

TEST_CASE("Nested blockquote", "[blockquote]")
{
	std::string markdown = R"md(> Im a blockquote!
>> In a blockquote!
> Last line)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 1);

	auto it = doc.begin();

	REQUIRE((*it)->getText() == 
		"Im a blockquote!\n"
		"In a blockquote!\n"
		"Last line"
	);
	REQUIRE((*it)->getHtml() == 
		"<blockquote><p>Im a blockquote!</p><blockquote><p>In a blockquote!</p></blockquote><p>Last line</p></blockquote>"
	);
}

TEST_CASE("Multiline nested blockquote", "[blockquote]")
{
	std::string markdown = R"md(> Level 1, line 1
> Level 1, line 2
>> Level 2, line 1
>> Level 2, line 2
> Level 1, line 3
> Level 1, line 4)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 1);

	auto it = doc.begin();

	REQUIRE((*it)->getText() ==
		"Level 1, line 1\n"
		"Level 1, line 2\n"
		"Level 2, line 1\n"
		"Level 2, line 2\n"
		"Level 1, line 3\n"
		"Level 1, line 4"
	);
	REQUIRE((*it)->getHtml() ==
		"<blockquote>"
		"<p>Level 1, line 1</p>"
		"<p>Level 1, line 2</p>"
		"<blockquote><p>Level 2, line 1</p>"
		"<p>Level 2, line 2</p></blockquote>"
		"<p>Level 1, line 3</p>"
		"<p>Level 1, line 4</p>"
		"</blockquote>"
	);
}

TEST_CASE("Document parsing blockquote and paragraph", "[blockquote]")
{
	std::string markdown = R"md(> Im a blockquote!
And regular paragraph)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getText() ==
		"Im a blockquote!\n"
		"And regular paragraph"
	);
	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body><blockquote><p>Im a blockquote!</p></blockquote><p>And regular paragraph</p></body></html>"
	);
}