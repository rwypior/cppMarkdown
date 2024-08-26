#include "codeelement.h"
#include "document.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Simple code block", "[code]")
{
	Markdown::CodeElement el("1337 code");

	REQUIRE(el.getText() == "1337 code");
	REQUIRE(el.getHtml() == "<code>1337 code</code>");

	Markdown::CodeElement el2("This is *some* code with __some special__ stuff");

	REQUIRE(el2.getText() == "This is *some* code with __some special__ stuff");
	REQUIRE(el2.getHtml() == "<code>This is *some* code with __some special__ stuff</code>");
}

TEST_CASE("Code block in document", "[code]")
{
	std::string markdown = R"md(Some paragraph

    This is a code block
And another paragraph)md";

	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 3);

	auto it = doc.begin();

	REQUIRE((*it)->getType() == Markdown::Type::Paragraph);
	it++;

	REQUIRE((*it)->getType() == Markdown::Type::Code);
	REQUIRE((*it)->getText() == "This is a code block");
	REQUIRE((*it)->getHtml() == "<code>This is a code block</code>");
	it++;

	REQUIRE((*it)->getType() == Markdown::Type::Paragraph);
}

TEST_CASE("Multiline code block", "[code]")
{
	std::string markdown = R"md(Some paragraph
    First line of code
	Second line of code
And another paragraph)md";

	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 3);

	auto it = doc.begin();

	REQUIRE((*it)->getType() == Markdown::Type::Paragraph);
	it++;

	REQUIRE((*it)->getType() == Markdown::Type::Code);
	REQUIRE((*it)->getText() == "First line of code\nSecond line of code");
	REQUIRE((*it)->getHtml() == "<code>First line of code\nSecond line of code</code>");
	it++;

	REQUIRE((*it)->getType() == Markdown::Type::Paragraph);
}

TEST_CASE("Code block with tabs", "[code]")
{
	std::string markdown = R"md(Some paragraph
	A code)md";

	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 2);

	auto it = doc.begin();

	REQUIRE((*it)->getType() == Markdown::Type::Paragraph);
	it++;

	REQUIRE((*it)->getType() == Markdown::Type::Code);
	REQUIRE((*it)->getText() == "A code");
	REQUIRE((*it)->getHtml() == "<code>A code</code>");
}