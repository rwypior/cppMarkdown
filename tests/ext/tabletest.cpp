#include "ext/tableelement.h"

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

TEST_CASE("Table with styled element", "[table]")
{
    std::string tableMarkdown =
        "A | B\n"
        "-|-\n"
        "1 | **bold** text\n";
    Markdown::TableElement table(tableMarkdown);

    REQUIRE(table.getCell(0, 0)->getText() == "1");
    REQUIRE(table.getCell(0, 1)->getHtml() == "<b>bold</b> text");
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