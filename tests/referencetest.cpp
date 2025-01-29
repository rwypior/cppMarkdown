#include "cppmarkdown/referenceelement.h"
#include "cppmarkdown/document.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Reference parsing", "[line]")
{
	Markdown::ReferenceElement el("[id]:value");
	REQUIRE(el.getId() == "id");
	REQUIRE(el.getValue() == "value");

	Markdown::ReferenceElement el2("[id]: value");
	REQUIRE(el2.getId() == "id");
	REQUIRE(el2.getValue() == "value");

	Markdown::ReferenceElement el3("[id]: value \"title\"");
	REQUIRE(el3.getId() == "id");
	REQUIRE(el3.getValue() == "value");
	REQUIRE(el3.getTitle() == "title");
	
	Markdown::ReferenceElement el4("[id]: value \'title\'");
	REQUIRE(el4.getId() == "id");
	REQUIRE(el4.getValue() == "value");
	REQUIRE(el4.getTitle() == "title");
}

TEST_CASE("Reference manager registration", "[line]")
{
	Markdown::ReferenceManager man;
	Markdown::ReferenceManager::ContextGuard cg(man);

	Markdown::ReferenceElement el("[id]:value");

	REQUIRE(man.getReference("id")->value == "value");
}

TEST_CASE("Reference in document", "[line]")
{
	{
		std::string markdown =
			"[id]: something\n"
			"[text][id]"
			;

		Markdown::Document doc;
		doc.parse(markdown);

		auto link = *std::next(doc.begin(), 1);

		REQUIRE(link->getHtml() == "<p><a href=\"something\">text</a></p>");
	}

	{
		std::string markdown =
			"[id]: something \"title\"\n"
			"[text][id]"
			;

		Markdown::Document doc;
		doc.parse(markdown);

		auto link = *std::next(doc.begin(), 1);

		REQUIRE(link->getHtml() == "<p><a href=\"something\" title=\"title\">text</a></p>");
	}

	{
		std::string markdown =
			"[id]: something\n"
			"![text][id]"
			;

		Markdown::Document doc;
		doc.parse(markdown);

		auto link = *std::next(doc.begin(), 1);

		REQUIRE(link->getHtml() == "<p><img src=\"something\" alt=\"text\"></p>");
	}

	{
		std::string markdown =
			"[id]: something 'A title'\n"
			"![text][id]"
			;

		Markdown::Document doc;
		doc.parse(markdown);

		auto link = *std::next(doc.begin(), 1);

		REQUIRE(link->getHtml() == "<p><img src=\"something\" alt=\"text\" title=\"A title\"></p>");
	}
}