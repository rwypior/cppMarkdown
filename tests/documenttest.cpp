#include "document.h"
#include "headingelement.h"
#include "paragraphelement.h"

#include <catch2/catch_all.hpp>

#include <functional>

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

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<p>paragraph 1</p>"
		"<p>paragraph 2</p>"
		"</body></html>"
	);
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

TEST_CASE("Document traversal", "[document]")
{
	std::string markdown = R"md(Heading
=====
paragraph)md";
	Markdown::Document doc;
	doc.parse(markdown);

	std::string html;
	std::function<void(Markdown::ElementContainer*)> traverse;

	traverse = [&html, &traverse](Markdown::ElementContainer* container) {
		if (auto* el = dynamic_cast<Markdown::Element*>(container))
		{
			html += el->getHtml() + "\n";
		}

		if (container)
		{
			for (auto& el : *container)
			{
				if (auto elcontainer = std::dynamic_pointer_cast<Markdown::ElementContainer>(el))
				{
					traverse(elcontainer.get());
				}
				else
				{
					html += el->getHtml() + "\n";
				}
			}
		}
	};
	traverse(&doc);
	html.pop_back();

	REQUIRE(html ==
		"<h1>Heading</h1>\n"
		"<p>paragraph</p>"
	);
}

TEST_CASE("Document construction", "[document]")
{
	Markdown::Document doc;
	auto h1 = std::make_shared<Markdown::HeadingElement>(Markdown::HeadingElement::Heading::Heading1, "This is heading");
	auto p = std::make_shared<Markdown::ParagraphElement>("And this is paragraph");
	doc.addElement(h1);
	doc.addElement(p);
	std::string html = doc.getHtml();

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<h1>This is heading</h1>"
		"<p>And this is paragraph</p>"
		"</body></html>"
	);
}