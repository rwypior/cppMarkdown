#include "cppmarkdown/paragraphelement.h"
#include "cppmarkdown/linebreakelement.h"
#include "cppmarkdown/document.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Parsing regular text", "[paragraph]")
{
	Markdown::ParagraphElement el("regular text");
	REQUIRE(el.getText() == "regular text");
	REQUIRE(el.getHtml() == "<p>regular text</p>");

	Markdown::ParagraphElement el2("");
	REQUIRE(el2.getText() == "");
	REQUIRE(el2.getHtml() == "");
}

TEST_CASE("Parsing markdown", "[paragraph]")
{
	Markdown::ParagraphElement el("*italic text*");
	REQUIRE(el.getText() == "italic text");
	REQUIRE(el.getHtml() == "<p><em>italic text</em></p>");

	Markdown::ParagraphElement el2("text **bold** text");
	REQUIRE(el2.getText() == "text bold text");
	REQUIRE(el2.getHtml() == "<p>text <strong>bold</strong> text</p>");
}

TEST_CASE("Multiple paragraphs", "[paragraph]")
{
	{
		std::string markdown = R"md(First
Second
Third)md";

		Markdown::Document doc;
		doc.parse(markdown);

		INFO(doc.getHtml());

		REQUIRE(doc.elementsCount() == 1);

		auto it = doc.begin();
		REQUIRE(std::static_pointer_cast<Markdown::ParagraphElement>(*it)->getText() == "First Second Third");
	}
	
	{
		std::string markdown = R"md(First

Second

Third)md";

		Markdown::Document doc;
		doc.parse(markdown);

		INFO(doc.getHtml());

		REQUIRE(doc.elementsCount() == 3);

		auto it = doc.begin();
		REQUIRE(std::static_pointer_cast<Markdown::ParagraphElement>(*it)->getText() == "First");

		it++;
		REQUIRE(std::static_pointer_cast<Markdown::ParagraphElement>(*it)->getText() == "Second");

		it++;
		REQUIRE(std::static_pointer_cast<Markdown::ParagraphElement>(*it)->getText() == "Third");
	}
}

TEST_CASE("Multiline paragraphs with styles", "[paragraph]")
{
	std::string markdown = R"md(First non-styled line
Second **styled line**
Third line non-styled)md";

	Markdown::Document doc;
	doc.parse(markdown);

	INFO(doc.getHtml());

	REQUIRE(doc.elementsCount() == 1);

	auto it = doc.begin();
	REQUIRE(std::static_pointer_cast<Markdown::ParagraphElement>(*it)->getHtml() == "<p>First non-styled line Second <strong>styled line</strong> Third line non-styled</p>");
}

TEST_CASE("Blank lines", "[paragraph]")
{
	std::string markdown = R"md(Some paragraph

Last line)md";

	Markdown::Document doc;
	doc.parse(markdown);

	INFO(doc.getHtml());
	REQUIRE(doc.elementsCount() == 2);

	auto it = doc.begin();
	auto first = std::static_pointer_cast<Markdown::ParagraphElement>(*it);
	REQUIRE(first->getText() == "Some paragraph");
	
	it++;
	auto third = std::static_pointer_cast<Markdown::ParagraphElement>(*it);
	REQUIRE(third->getText() == "Last line");
}

TEST_CASE("Getting markdown", "[paragraph]")
{
	{
		Markdown::ParagraphElement el("Some paragraph with **a style**");

		REQUIRE(el.getMarkdown() == "Some paragraph with **a style**");
	}

	{
		Markdown::ParagraphElement el("Some paragraph with **_a style_**");

		REQUIRE(el.getMarkdown() == "Some paragraph with **_a style_**");
	}

	{
		Markdown::ParagraphElement el("Some paragraph with **__weird double-bold mix__**");

		REQUIRE(el.getMarkdown() == "Some paragraph with **__weird double-bold mix__**");
	}
}