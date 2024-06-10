#include "cppmarkdown.h"

#include <catch/catch.hpp>

TEST_CASE("Parsing regular text", "[paragraph]")
{
	Markdown::ParagraphElement para("regular text");
	REQUIRE(para.getText() == "regular text");
	REQUIRE(para.getHtml() == "regular text");

	Markdown::ParagraphElement para2("");
	REQUIRE(para2.getText() == "");
	REQUIRE(para2.getHtml() == "");
}

TEST_CASE("Parsing markdown", "[paragraph]")
{
	Markdown::ParagraphElement para("*italic text*");
	REQUIRE(para.getText() == "italic text");
	REQUIRE(para.getHtml() == "<i>italic text</i>");

	Markdown::ParagraphElement para2("text **bold** text");
	REQUIRE(para2.getText() == "text bold text");
	REQUIRE(para2.getHtml() == "text <b>bold</b> text");
}