#include "cppmarkdown/listelement.h"

#include <catch2/catch_all.hpp>

namespace Catch
{
	template<>
	struct StringMaker<std::tuple<size_t, size_t>>
	{
		static std::string convert(std::tuple<size_t, size_t> const& value)
		{
			return "{" + std::to_string(std::get<0>(value)) + ", " + std::to_string(std::get<1>(value)) + "}";
		}
	};

	template<>
	struct StringMaker<Markdown::ListElement::ListMarker>
	{
		static std::string convert(Markdown::ListElement::ListMarker const& value)
		{
			return "ListMarker { type=" + std::to_string(static_cast<int>(value.type)) + ", level=" + std::to_string(value.level) + "}";
		}
	};
}

TEST_CASE("List leading spaces", "[list]")
{
	using namespace Markdown;

	REQUIRE(ListElement::countLeadingSpaces("x") == 0);
	REQUIRE(ListElement::countLeadingSpaces("    x") == 4);
}

TEST_CASE("List level parsing", "[list]")
{
	using namespace Markdown;

	// Not a list
	REQUIRE(ListElement::getListLevel("x") == ListElement::ListMarker(0, ListElement::ListType::Invalid));
	REQUIRE(ListElement::getListLevel("    x") == ListElement::ListMarker(1, ListElement::ListType::Invalid));
	REQUIRE(ListElement::getListLevel("\tx") == ListElement::ListMarker(1, ListElement::ListType::Invalid));

	// Ordered list
	REQUIRE(ListElement::getListLevel("1. x") == ListElement::ListMarker(0, ListElement::ListType::Ordered));
	REQUIRE(ListElement::getListLevel("    1. x") == ListElement::ListMarker(1, ListElement::ListType::Ordered));
	REQUIRE(ListElement::getListLevel("        1. x") == ListElement::ListMarker(2, ListElement::ListType::Ordered));
	REQUIRE(ListElement::getListLevel("\t1. x") == ListElement::ListMarker(1, ListElement::ListType::Ordered));
	REQUIRE(ListElement::getListLevel("\t\t1. x") == ListElement::ListMarker(2, ListElement::ListType::Ordered));

	// Unordered list
	REQUIRE(ListElement::getListLevel("- x") == ListElement::ListMarker(0, ListElement::ListType::Unordered));
	REQUIRE(ListElement::getListLevel("    - x") == ListElement::ListMarker(1, ListElement::ListType::Unordered));
	REQUIRE(ListElement::getListLevel("        - x") == ListElement::ListMarker(2, ListElement::ListType::Unordered));
	REQUIRE(ListElement::getListLevel("\t- x") == ListElement::ListMarker(1, ListElement::ListType::Unordered));
	REQUIRE(ListElement::getListLevel("\t\t- x") == ListElement::ListMarker(2, ListElement::ListType::Unordered));

	// Invalid
	REQUIRE(!ListElement::getListLevel("This is some text - with hyphen"));
	REQUIRE(!ListElement::getListLevel("And same text with * asterisk"));
}

TEST_CASE("List find ordered marker", "[list]")
{
	using namespace Markdown;
	using poslen = std::tuple<size_t, size_t>;
	constexpr poslen invalid = { std::string::npos, std::string::npos };

	REQUIRE(ListElement::findOrderedMarker("123. something") == poslen{3, 3});
	REQUIRE(ListElement::findOrderedMarker("    123. offset") == poslen{7, 3});
	REQUIRE(ListElement::findOrderedMarker("    123. 456. offset") == poslen{7, 3});
	REQUIRE(ListElement::findOrderedMarker("123. something -something else") == poslen{3, 3});
	REQUIRE(ListElement::findOrderedMarker("123. something 456. another something") == poslen{3, 3});

	REQUIRE(ListElement::findOrderedMarker("  42  123. offset") == invalid);
	REQUIRE(ListElement::findOrderedMarker("  x  123. offset") == invalid);
	REQUIRE(ListElement::findOrderedMarker("    x. offset") == invalid);
}

TEST_CASE("List find unordered marker", "[list]")
{
	using namespace Markdown;

	REQUIRE(ListElement::findUnorderedMarker("- something") == 0);
	REQUIRE(ListElement::findUnorderedMarker("    - offset") == 4);
	REQUIRE(ListElement::findUnorderedMarker("  -  - half offset") == 2);
	REQUIRE(ListElement::findUnorderedMarker("- something .1") == 0);

	REQUIRE(ListElement::findUnorderedMarker("  42 - offset") == std::string::npos);
	REQUIRE(ListElement::findUnorderedMarker("  x  - offset") == std::string::npos);
	REQUIRE(ListElement::findUnorderedMarker("something -") == std::string::npos);
}

TEST_CASE("List text parsing", "[list]")
{
	using namespace Markdown;

	REQUIRE(ListElement::getListText("1.Something") == "Something");
	REQUIRE(ListElement::getListText("1. Something") == "Something");
	REQUIRE(ListElement::getListText("Something") == "");
	REQUIRE(ListElement::getListText("1.   Something") == "Something");
	REQUIRE(ListElement::getListText("    1. Something") == "Something");
	REQUIRE(ListElement::getListText("        1. Something") == "Something");
	REQUIRE(ListElement::getListText("        1.   Something") == "Something");
	REQUIRE(ListElement::getListText("123.Something") == "Something");

	REQUIRE(ListElement::getListTextWithMarker("1.Something") == "1.Something");
	REQUIRE(ListElement::getListTextWithMarker("1. Something") == "1. Something");
	REQUIRE(ListElement::getListTextWithMarker("Something") == "");
	REQUIRE(ListElement::getListTextWithMarker("1.   Something") == "1.   Something");
	REQUIRE(ListElement::getListTextWithMarker("123.   Something") == "123.   Something");
	REQUIRE(ListElement::getListTextWithMarker("    123. Something") == "123. Something");
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
	INFO(el.dump());

	REQUIRE(el.size() == 3);
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

	REQUIRE(el.size() == 3);
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

	REQUIRE(el.size() == 3);
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
    Paragraph)list");
	INFO(el.dump());

	REQUIRE(el.getHtml() == "<ol>"
		"<li>First</li>"
		"<li>Second<ol>"
		"<li>Nested 1</li>"
		"<li>Nested 2</li>"
		"</ol>Paragraph</li></ol>");
}

TEST_CASE("Simple list in document", "[list]")
{
	std::string markdown = R"md(1. First
2. Second
3. Third)md";
	Markdown::Document doc;
	doc.parse(markdown);

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

TEST_CASE("Text before and after list", "[list]")
{
	std::string markdown = R"md(Some text
- First
- Second
- Third

Some stuff)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<p>Some text</p>"
		"<ul>"
		"<li>First</li>"
		"<li>Second</li>"
		"<li>Third</li>"
		"</ul>"
		"<br>"
		"<p>Some stuff</p>"
		"</body></html>"
	);
}

TEST_CASE("List with elements with dots", "[list]")
{
	std::string markdown = R"md(- First
- Second
- Third...)md";
	Markdown::Document doc;
	doc.parse(markdown);

	REQUIRE(doc.getHtml() ==
		"<!DOCTYPE html><html><head></head><body>"
		"<ul>"
		"<li>First</li>"
		"<li>Second</li>"
		"<li>Third...</li>"
		"</ul>"
		"</body></html>"
	);
}