#include "ext/tableelement.h"

#include <catch2/catch_all.hpp>

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
}
