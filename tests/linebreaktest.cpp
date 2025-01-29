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