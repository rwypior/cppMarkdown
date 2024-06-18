#include "listelement.h"

#include <catch2/catch.hpp>

TEST_CASE("Simple list", "[list]")
{
	Markdown::ListElement el(R"list(1. First
2. Second
3. Third)list");
	REQUIRE(el.getText() == R"list(1. First
2. Second
3. Third)list");
	REQUIRE(el.getHtml() == R"list(<ol><li>First</li><li>Second</li><li>Third</li></ol>)list");
}

TEST_CASE("List with paragraph", "[list]")
{
	Markdown::ListElement el(R"list(1. First
2. Second
3. Third
    Paragraph)list");
	REQUIRE(el.elements.elementsCount() == 3);
	REQUIRE(el.getText() == R"list(1. First
2. Second
3. Third
Paragraph)list");
	REQUIRE(el.getHtml() == R"list(<ol><li>First</li><li>Second</li><li>Third<br>Paragraph</li></ol>)list");
}

TEST_CASE("Simple list in document", "[list]")
{
	std::string markdown = R"md(1. First
2. Second
3. Third)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 1);

	auto it = doc.begin();

	REQUIRE((*it)->getText() ==
		"1. First\n"
		"2. Second\n"
		"3. Third"
	);
	REQUIRE((*it)->getHtml() ==
		"<ol><li>First</li><li>Second</li><li>Third</li></ol>"
	);
}