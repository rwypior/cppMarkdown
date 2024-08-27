#include "paragraphelement.h"
#include "linebreakelement.h"
#include "document.h"

#include <catch2/catch_all.hpp>

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
	REQUIRE(el.getHtml() == "<p><i>italic text</i></p>");

	Markdown::ParagraphElement el2("text **bold** text");
	REQUIRE(el2.getText() == "text bold text");
	REQUIRE(el2.getHtml() == "<p>text <b>bold</b> text</p>");
}

TEST_CASE("Multiple paragraphs", "[paragraph]")
{
	std::string markdown = R"md(First
Second
Third)md";

	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 3);

	auto it = doc.begin();
	REQUIRE(std::static_pointer_cast<Markdown::ParagraphElement>(*it)->getText() == "First");

	it++;
	REQUIRE(std::static_pointer_cast<Markdown::ParagraphElement>(*it)->getText() == "Second");

	it++;
	REQUIRE(std::static_pointer_cast<Markdown::ParagraphElement>(*it)->getText() == "Third");
}

TEST_CASE("Blank lines", "[paragraph]")
{
	std::string markdown = R"md(Some paragraph

Last line)md";

	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 3);

	auto it = doc.begin();
	auto first = std::static_pointer_cast<Markdown::ParagraphElement>(*it);
	REQUIRE(first->getText() == "Some paragraph");

	it++;
	auto second = std::static_pointer_cast<Markdown::LineBreakElement>(*it);
	REQUIRE(second);
	
	it++;
	auto third = std::static_pointer_cast<Markdown::ParagraphElement>(*it);
	REQUIRE(third->getText() == "Last line");
}