#include "cppmarkdown/lineelement.h"
#include "cppmarkdown/linebreakelement.h"
#include "cppmarkdown/document.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Simple line", "[line]")
{
	Markdown::LineElement el;
	REQUIRE(el.getType() == Markdown::Type::Line);
	REQUIRE(el.getText() == "---");
	REQUIRE(el.getHtml() == "<hr>");
	
	Markdown::LineElement el2(10, 'x');
	REQUIRE(el2.getType() == Markdown::Type::Line);
	REQUIRE(el2.getText() == "xxxxxxxxxx");
	REQUIRE(el2.getHtml() == "<hr>");
}

TEST_CASE("Line in document", "[code]")
{
	std::string markdown = R"md(Some paragraph

---
And another paragraph)md";

	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<p>Some paragraph</p>"
		"<br>"
		"<hr>"
		"<p>And another paragraph</p>"
		"</body></html>"
	);
}

TEST_CASE("Two lines in document", "[code]")
{
	std::string markdown = R"md(Some paragraph

---
---)md";

	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 4);

	auto it = doc.begin();

	REQUIRE((*it)->getType() == Markdown::Type::Paragraph);
	it++;

	REQUIRE((*it)->getType() == Markdown::Type::LineBreak);
	it++;

	REQUIRE((*it)->getType() == Markdown::Type::Line);
	it++;

	REQUIRE((*it)->getType() == Markdown::Type::Line);
}

TEST_CASE("Line types", "[code]")
{
	Markdown::Document doc1;
	doc1.parse("---");
	REQUIRE((*doc1.begin())->getType() == Markdown::Type::Line);

	Markdown::Document doc2;
	doc2.parse("***");
	REQUIRE((*doc2.begin())->getType() == Markdown::Type::Line);

	Markdown::Document doc3;
	doc3.parse("___");
	REQUIRE((*doc3.begin())->getType() == Markdown::Type::Line);
}