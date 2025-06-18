#include "cppmarkdown/codeelement.h"
#include "cppmarkdown/document.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Code element level counting", "[code]")
{
	REQUIRE(Markdown::CodeElement::getCodeLevel("\txxx") == 1);
	REQUIRE(Markdown::CodeElement::getCodeLevel("\t        xxx") == 1);
	REQUIRE(Markdown::CodeElement::getCodeLevel("\t\txxx") == 2);
	REQUIRE(Markdown::CodeElement::getCodeLevel("\t\t        xxx") == 2);
	REQUIRE(Markdown::CodeElement::getCodeLevel("   xxx") == 0);
	REQUIRE(Markdown::CodeElement::getCodeLevel("    xxx") == 1);
	REQUIRE(Markdown::CodeElement::getCodeLevel("        xxx") == 2);
	REQUIRE(Markdown::CodeElement::getCodeLevel("        \txxx") == 2);
	REQUIRE(Markdown::CodeElement::getCodeLevel("") == 0);
	REQUIRE(Markdown::CodeElement::getCodeLevel("xxx") == 0);
	REQUIRE(Markdown::CodeElement::getCodeLevel("xxx    ") == 0);
	REQUIRE(Markdown::CodeElement::getCodeLevel("xxx\t") == 0);
}

TEST_CASE("Simple code block", "[code]")
{
	Markdown::CodeElement el("1337 code");

	REQUIRE(el.getText() == "1337 code");
	REQUIRE(el.getHtml() == "<pre><code>1337 code</code></pre>");

	Markdown::CodeElement el2("This is *some* code with __some special__ stuff");

	REQUIRE(el2.getText() == "This is *some* code with __some special__ stuff");
	REQUIRE(el2.getHtml() == "<pre><code>This is *some* code with __some special__ stuff</code></pre>");
}

TEST_CASE("Lone code block in document", "[code]")
{
	std::string markdown = R"md(    This is a code block)md";

	Markdown::Document doc;
	doc.parse(markdown);

	INFO(doc.dump());

	REQUIRE(doc.elementsCount() == 1);

	auto it = doc.begin();
	REQUIRE((*it)->getType() == Markdown::Type::Code);
	REQUIRE((*it)->getText() == "This is a code block");
	REQUIRE((*it)->getHtml() == "<pre><code>This is a code block</code></pre>");
}

TEST_CASE("Code block in document", "[code]")
{
	std::string markdown = R"md(Some paragraph

    This is a code block
And another paragraph)md";

	Markdown::Document doc;
	doc.parse(markdown);

	INFO(doc.dump());

	REQUIRE(doc.elementsCount() == 3);

	auto it = doc.begin();

	REQUIRE((*it)->getType() == Markdown::Type::Paragraph);
	it++;

	REQUIRE((*it)->getType() == Markdown::Type::Code);
	REQUIRE((*it)->getText() == "This is a code block");
	REQUIRE((*it)->getHtml() == "<pre><code>This is a code block</code></pre>");
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

	INFO(doc.dump());

	REQUIRE(doc.elementsCount() == 3);

	auto it = doc.begin();

	REQUIRE((*it)->getType() == Markdown::Type::Paragraph);
	it++;

	REQUIRE((*it)->getType() == Markdown::Type::Code);
	REQUIRE((*it)->getText() == "First line of code\nSecond line of code");
	REQUIRE((*it)->getHtml() == "<pre><code>First line of code\nSecond line of code</code></pre>");
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
	REQUIRE((*it)->getHtml() == "<pre><code>A code</code></pre>");
}