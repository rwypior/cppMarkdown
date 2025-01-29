#include "cppmarkdown/headingelement.h"
#include "cppmarkdown/document.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Parsing standard heading", "[heading]")
{
	Markdown::HeadingElement el(Markdown::HeadingElement::Heading::Heading1, "Heading!");
	
	REQUIRE(el.getText() == "Heading!");
	REQUIRE(el.getHtml() == "<h1>Heading!</h1>");
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