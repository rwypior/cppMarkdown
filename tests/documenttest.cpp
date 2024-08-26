#include "document.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Single paragraph", "[document]")
{
	std::string markdown = R"md(paragraph 1)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 1);

	auto it = doc.begin();

	REQUIRE((*it)->getText() == "paragraph 1");
}

TEST_CASE("Two paragraphs", "[document]")
{
	std::string markdown = R"md(paragraph 1
paragraph 2)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 2);

	auto it = doc.begin();

	REQUIRE((*it)->getText() == "paragraph 1");
	it++;
	REQUIRE((*it)->getText() == "paragraph 2");
}

TEST_CASE("Complex document", "[document]")
{
	std::string markdown = R"md(First paragraph
Second paragraph
### Title3
Third paragraph
Alternate header 1
===)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.elementsCount() == 5);

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<p>First paragraph</p>"
		"<p>Second paragraph</p>"
		"<h3>Title3</h3>"
		"<p>Third paragraph</p>"
		"<h1>Alternate header 1</h1>"
		"</body></html>"
	);
}