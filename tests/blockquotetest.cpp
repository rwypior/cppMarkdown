#include "blockquoteelement.h"

#include <catch/catch.hpp>

TEST_CASE("Simple blockquote", "[blockquote]")
{
	Markdown::BlockquoteElement para("> Im a blockquote!");
	REQUIRE(para.getText() == "Im a blockquote!");
	REQUIRE(para.getHtml() == "<blockquote>Im a blockquote!</blockquote>");
}

TEST_CASE("Blockquote and paragraph", "[blockquote]")
{
	Markdown::BlockquoteElement para(
		"> Im a blockquote!\n"
		"And regular paragraph"
	);
	REQUIRE(para.getText() == 
		"Im a blockquote!\n"
		"And regular paragraph");
	REQUIRE(para.getHtml() == 
		"<blockquote>Im a blockquote!</blockquote>\n"
		"<p>And regular paragraph</p>"
	);
}

TEST_CASE("Multiline blockquote", "[blockquote]")
{
	Markdown::BlockquoteElement para(
		"> Im a blockquote!\n"
		"> With two lines!"
	);
	REQUIRE(para.getText() == 
		"Im a blockquote!\n"
		"With two lines!");
	REQUIRE(para.getHtml() == 
		"<blockquote>Im a blockquote!\n"
		"With two lines!</blockquote>");
}

TEST_CASE("Nested blockquote", "[blockquote]")
{
	Markdown::BlockquoteElement para(
		"> Im a blockquote!\n"
		">> In a blockquote!"
	);
	REQUIRE(para.getText() == 
		"Im a blockquote!\n"
		"In a blockquote!");
	REQUIRE(para.getHtml() == 
		"<blockquote>Im a blockquote!\n"
		"<blockquote>In a blockquote!</blockquote>"
		"</blockquote>");
}