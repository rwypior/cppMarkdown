#include "listelement.h"

#include <catch2/catch_all.hpp>

TEST_CASE("List level parsing", "[list]")
{
	// Not a list
	REQUIRE(Markdown::ListElement::getListLevel("x").level == -1);

	// Ordered list
	REQUIRE(Markdown::ListElement::getListLevel("1. x").level == 0);
	REQUIRE(Markdown::ListElement::getListLevel("    1. x").level == 1);
	REQUIRE(Markdown::ListElement::getListLevel("        1. x").level == 2);
	REQUIRE(Markdown::ListElement::getListLevel("\t1. x").level == 1);
	REQUIRE(Markdown::ListElement::getListLevel("\t\t1. x").level == 2);

	// Unordered list
	REQUIRE(Markdown::ListElement::getListLevel("- x").level == 0);
	REQUIRE(Markdown::ListElement::getListLevel("    - x").level == 1);
	REQUIRE(Markdown::ListElement::getListLevel("        - x").level == 2);
	REQUIRE(Markdown::ListElement::getListLevel("\t- x").level == 1);
	REQUIRE(Markdown::ListElement::getListLevel("\t\t- x").level == 2);
}

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

TEST_CASE("Simple unordered list", "[list]")
{
	Markdown::ListElement el(R"list(* First
* Second
* Third)list");
	REQUIRE(el.getText() == R"list(- First
- Second
- Third)list");
	REQUIRE(el.getHtml() == R"list(<ul><li>First</li><li>Second</li><li>Third</li></ul>)list");
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

TEST_CASE("List with paragraph in the middle", "[list]")
{
	Markdown::ListElement el(R"list(1. First
2. Second
    Paragraph
3. Third)list");
	REQUIRE(el.elements.elementsCount() == 3);
	REQUIRE(el.getText() == R"list(1. First
2. Second
Paragraph
3. Third)list");
	REQUIRE(el.getHtml() == R"list(<ol><li>First</li><li>Second<br>Paragraph</li><li>Third</li></ol>)list");
}

TEST_CASE("List with multiline paragraph in the middle", "[list]")
{
	Markdown::ListElement el(R"list(1. First
2. Second
    Paragraph
    Another line
3. Third)list");
	REQUIRE(el.elements.elementsCount() == 3);
	REQUIRE(el.getText() == R"list(1. First
2. Second
Paragraph
Another line
3. Third)list");
	REQUIRE(el.getHtml() == R"list(<ol><li>First</li><li>Second<br>Paragraph<br>Another line</li><li>Third</li></ol>)list");
}

TEST_CASE("Document with list with multiline paragraph in the middle", "[list]")
{
	std::string markdown = R"md(1. First
2. Second
    Paragraph
    Another line
3. Third)md";
	Markdown::Document doc;
	doc.parse(markdown);
	REQUIRE(doc.elementsCount() == 1);

	auto it = doc.begin();
	REQUIRE((*it)->getText() == R"list(1. First
2. Second
Paragraph
Another line
3. Third)list");
	REQUIRE((*it)->getHtml() == R"list(<ol><li>First</li><li>Second<br>Paragraph<br>Another line</li><li>Third</li></ol>)list");
}

TEST_CASE("Nested list", "[list]")
{
	Markdown::ListElement el(R"list(1. First
2. Second
    1. Nested 1
    2. Nested 2
    3. Nested 3
    Paragraph)list");
	std::string dump = el.dump();
	REQUIRE(el.elements.elementsCount() == 3);
	REQUIRE(el.getText() == R"list(1. First
2. Second
1. Nested 1
2. Nested 2
3. Nested 3
Paragraph)list");
	REQUIRE(el.getHtml() == "<ol>"
"<li>First</li>"
"<li>Second<ol>"
	"<li>Nested 1</li>"
	"<li>Nested 2</li>"
	"<li>Nested 3</li>"
"</ol>Paragraph</li></ol>");
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