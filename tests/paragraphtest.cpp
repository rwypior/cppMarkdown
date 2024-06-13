#include "paragraphelement.h"

#include <catch2/catch.hpp>

TEST_CASE("Parsing regular text", "[paragraph]")
{
	Markdown::ParagraphElement el("regular text");
	REQUIRE(el.getText() == "regular text");
	REQUIRE(el.getHtml() == "<p>regular text</p>");

	Markdown::ParagraphElement el2("");
	REQUIRE(el2.getText() == "");
	REQUIRE(el2.getHtml() == "");
}

TEST_CASE("Parsing markdown", "[paragraph]")
{
	Markdown::ParagraphElement el("*italic text*");
	REQUIRE(el.getText() == "italic text");
	REQUIRE(el.getHtml() == "<i>italic text</i>");

	Markdown::ParagraphElement el2("text **bold** text");
	REQUIRE(el2.getText() == "text bold text");
	REQUIRE(el2.getHtml() == "<p>text <b>bold</b> text</p>");
}