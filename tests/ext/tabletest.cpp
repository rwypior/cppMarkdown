#include "cppmarkdown/ext/tableelement.h"
#include "cppmarkdown/extensions.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Table row parsing", "[table]")
{
    std::string line = "qwe | asd | zxc";
    auto row = Markdown::TableElement::parseRow(line);
    REQUIRE(row.size() == 3);
    REQUIRE(row.at(0).getText() == "qwe");
    REQUIRE(row.at(1).getText() == "asd");
    REQUIRE(row.at(2).getText() == "zxc");

    std::string line2 = "qwe |";
    auto row2 = Markdown::TableElement::parseRow(line2);
    REQUIRE(row2.size() == 1);
    REQUIRE(row2.at(0).getText() == "qwe");
}

TEST_CASE("Table line validation", "[table]")
{
    REQUIRE(Markdown::TableElement::tableLineValid("-|-", 2));
    REQUIRE(Markdown::TableElement::tableLineValid("--|--", 2));

    REQUIRE(Markdown::TableElement::tableLineValid("-|-|-", 3));
    REQUIRE(Markdown::TableElement::tableLineValid("---|--|-", 3));
    REQUIRE(Markdown::TableElement::tableLineValid(" ---|--|-  ", 3));

    REQUIRE(Markdown::TableElement::tableLineValid("-|", 1));
    REQUIRE(Markdown::TableElement::tableLineValid("---|", 1));
    REQUIRE(Markdown::TableElement::tableLineValid(" ---|   ", 1));

    REQUIRE(!Markdown::TableElement::tableLineValid("|", 1));
    REQUIRE(!Markdown::TableElement::tableLineValid("||", 3));
    REQUIRE(!Markdown::TableElement::tableLineValid("-||", 3));
    REQUIRE(!Markdown::TableElement::tableLineValid("|-|-", 3));
}

TEST_CASE("Table parse columns", "[table]")
{
    REQUIRE(Markdown::TableElement::parseColumnCount("-|-|-") == 3);
    REQUIRE(Markdown::TableElement::parseColumnCount("-|-") == 2);
    REQUIRE(Markdown::TableElement::parseColumnCount("-|") == 1);
}

TEST_CASE("Table parsing", "[table]")
{
    std::string tableMarkdown =
    "this|is|header\n"
    "----|--|------\n"
    "1   |2 |3     \n"
    "a   |b |c     \n";
    Markdown::TableElement table(tableMarkdown);

    REQUIRE(table.columnCount() == 3);
    REQUIRE(table.rowCount() == 2);

    REQUIRE(table.getHeaderCell(0)->getText() == "this");
    REQUIRE(table.getHeaderCell(1)->getText() == "is");
    REQUIRE(table.getHeaderCell(2)->getText() == "header");

    REQUIRE(table.getCell(0, 0)->getText() == "1");
    REQUIRE(table.getCell(0, 1)->getText() == "2");
    REQUIRE(table.getCell(0, 2)->getText() == "3");

    REQUIRE(table.getCell(1, 0)->getText() == "a");
    REQUIRE(table.getCell(1, 1)->getText() == "b");
    REQUIRE(table.getCell(1, 2)->getText() == "c");
}

TEST_CASE("Table parsing to text", "[table]")
{
    std::string tableMarkdown =
        "this|is|header\n"
        "-|-|-\n"
        "1234567|2|3\n"
        "a|b|c\n";
    Markdown::TableElement table(tableMarkdown);

    REQUIRE(table.getText() == 
        "this    | is | header \n"
        "--------|----|--------\n"
        "1234567 | 2  | 3      \n"
        "a       | b  | c      "
    );
}

TEST_CASE("Table with styled element", "[table]")
{
    std::string tableMarkdown =
        "A | B\n"
        "-|-\n"
        "1 | **bold** text\n";
    Markdown::TableElement table(tableMarkdown);

    REQUIRE(table.getCell(0, 0)->getText() == "1");
    REQUIRE(table.getCell(0, 1)->getHtml() == "<strong>bold</strong> text");
}

TEST_CASE("Table single column", "[table]")
{
    std::string tableMarkdown =
        "A |\n"
        "-|\n"
        "Single|\n";
    Markdown::TableElement table(tableMarkdown);

    REQUIRE(table.columnCount() == 1);
    REQUIRE(table.rowCount() == 1);

    REQUIRE(table.getCell(0, 0)->getText() == "Single");
}

TEST_CASE("Table parsing in document", "[table]")
{
    Markdown::registerStandardExtensions();

    std::string tableMarkdown =
        "A   |B \n"
        "----|--\n"
        "1   |2 \n"
        "a   |b \n";
    Markdown::Document doc;
    doc.parse(tableMarkdown);

    REQUIRE(doc.getHtml() == 
        "<!DOCTYPE html><html><head></head><body>"
        "<table>"
        "<tr><th>A</th><th>B</th></tr>"
        "<tr><td>1</td><td>2</td></tr>"
        "<tr><td>a</td><td>b</td></tr>"
        "</table>"
        "</body></html>"
    );
}

TEST_CASE("Table parsing in complex document", "[table]")
{
    Markdown::registerStandardExtensions();

    std::string tableMarkdown =
        "Something blabla\n"
        "\n"
        "A   |B \n"
        "----|--\n"
        "1   |2 \n"
        "a   |b \n"
        "\n"
        "Stuff\n";
    Markdown::Document doc;
    doc.parse(tableMarkdown);

    REQUIRE(doc.getHtml() ==
        "<!DOCTYPE html><html><head></head><body>"
        "<p>Something blabla</p>"
        "<br>" // Do we want this line break here?
        "<table>"
        "<tr><th>A</th><th>B</th></tr>"
        "<tr><td>1</td><td>2</td></tr>"
        "<tr><td>a</td><td>b</td></tr>"
        "</table>"
        "<p>Stuff</p>"
        "</body></html>"
    );
}

TEST_CASE("Table header only", "[table]")
{
    Markdown::registerStandardExtensions();

    std::string tableMarkdown =
        "A   |B \n";
    Markdown::Document doc;
    doc.parse(tableMarkdown);

    REQUIRE(doc.getHtml() ==
        "<!DOCTYPE html><html><head></head><body>"
        "<table>"
        "<tr><th>A</th><th>B</th></tr>"
        "</table>"
        "</body></html>"
    );
}

TEST_CASE("Incomplete table", "[table]")
{
    Markdown::registerStandardExtensions();

    std::string tableMarkdown =
        "A   |B \n";
        "----|--\n";
    Markdown::Document doc;
    doc.parse(tableMarkdown);

    REQUIRE(doc.getHtml() ==
        "<!DOCTYPE html><html><head></head><body>"
        "<table>"
        "<tr><th>A</th><th>B</th></tr>"
        "</table>"
        "</body></html>"
    );
}

TEST_CASE("Document without table", "[table]")
{
    Markdown::registerStandardExtensions();

    std::string tableMarkdown = "Whatever";
    Markdown::Document doc;
    doc.parse(tableMarkdown);

    REQUIRE(doc.getHtml() ==
        "<!DOCTYPE html><html><head></head><body>"
        "<p>Whatever</p>"
        "</body></html>"
    );
}