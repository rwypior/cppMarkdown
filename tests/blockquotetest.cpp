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
>> In a blockquote!)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 1);

	auto it = doc.begin();

	REQUIRE((*it)->getText() == 
		"Im a blockquote!\n"
		"In a blockquote!"
	);
	REQUIRE((*it)->getHtml() == 
		"<blockquote><p>Im a blockquote!</p><blockquote><p>In a blockquote!</p></blockquote></blockquote>"
	);
}

TEST_CASE("Document parsing blockquote and paragraph", "[blockquote]")
{
	return;

	Markdown::BlockquoteElement el(
		"> Im a blockquote!\n"
		"And regular paragraph"
	);
	REQUIRE(el.getText() ==
		"Im a blockquote!\n"
		"And regular paragraph");
	REQUIRE(el.getHtml() ==
		"<blockquote>Im a blockquote!</blockquote>\n"
		"<p>And regular paragraph</p>"
	);
}