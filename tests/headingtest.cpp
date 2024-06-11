#include "headingelement.h"
#include "document.h"

#include <catch2/catch.hpp>

TEST_CASE("Parsing standard heading", "[heading]")
{
	Markdown::HeadingElement head(Markdown::HeadingElement::Heading::Heading1, "Heading!");
	
	REQUIRE(head.getText() == "Heading!");
	REQUIRE(head.getHtml() == "<h1>Heading!</h1>");
}

TEST_CASE("Paragraph after heading", "[heading]")
{
	std::string markdown = R"md(A paragraph
### A heading)md";

	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<p>A paragraph</p>"
		"<h3>A heading</h3>"
		"</body></html>"
	);
}

TEST_CASE("Parsing alternate heading", "[heading]")
{
	std::string markdown = R"md(This is header
-----)md";

	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getHtml() == 
		"<!DOCTYPE html><html><head></head><body>"
		"<h2>This is header</h2>"
		"</body></html>"
	);
}

TEST_CASE("Parsing alternate heading single char", "[heading]")
{
	std::string markdown = R"md(This is header
=)md";

	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<h1>This is header</h1>"
		"</body></html>"
	);
}