#include "cppmarkdown.h"

#include <catch/catch.hpp>

TEST_CASE("test", "[document]")
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