#include "document.h"
#include "blankelement.h"
#include "lineelement.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Prepend blank elements with new lines", "[elementcontainer]")
{
		Markdown::ElementContainer container;
		container.addElement(std::make_shared<Markdown::BlankElement>("Element 1"));
		container.addElement(std::make_shared<Markdown::BlankElement>("Element 2"));
		container.addElement(std::make_shared<Markdown::BlankElement>("Element 3"));

		container.iterate(Markdown::prependBlankElementsWithNewlines);

		REQUIRE(container.at(0)->getText() == "Element 1");
		REQUIRE(container.at(1)->getType() == Markdown::Type::LineBreak);
		REQUIRE(container.at(2)->getText() == "Element 2");
		REQUIRE(container.at(3)->getType() == Markdown::Type::LineBreak);
		REQUIRE(container.at(4)->getText() == "Element 3");
}

TEST_CASE("Prepend blank elements with new lines with other elements between", "[elementcontainer]")
{
	Markdown::ElementContainer container;
	container.addElement(std::make_shared<Markdown::BlankElement>("Element 1"));
	container.addElement(std::make_shared<Markdown::LineElement>());
	container.addElement(std::make_shared<Markdown::BlankElement>("Element 2"));
	container.addElement(std::make_shared<Markdown::BlankElement>("Element 3"));
	container.addElement(std::make_shared<Markdown::LineElement>());

	container.iterate(Markdown::prependBlankElementsWithNewlines);

	REQUIRE(container.at(0)->getText() == "Element 1");
	REQUIRE(container.at(1)->getType() == Markdown::Type::Line);
	REQUIRE(container.at(2)->getText() == "Element 2");
	REQUIRE(container.at(3)->getType() == Markdown::Type::LineBreak);
	REQUIRE(container.at(4)->getText() == "Element 3");
	REQUIRE(container.at(5)->getType() == Markdown::Type::Line);
}

TEST_CASE("Prepend blank elements with new lines on empty container", "[elementcontainer]")
{
	Markdown::ElementContainer container;
	container.iterate(Markdown::prependBlankElementsWithNewlines);
	REQUIRE(container.size() == 0);
}