#include "cppmarkdown/ext/tableelement.h"
#include "cppmarkdown/extensions.h"
#include "cppmarkdown/html.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Pretty table", "[table]")
{
    {
        // Test pretty table
        Markdown::SubstituteHtmlProvider<Markdown::PrettyTableProvider> prettyTable;

        std::string tableMarkdown =
            "this|is|header\n"
            "----|--|------\n"
            "1   |2 |3     \n";
        Markdown::TableElement table(tableMarkdown);

        REQUIRE(table.getHtml() == 
            "<table cellpadding=\"10\" border=\"1\" rules=\"all\">"
            "<tr valign=\"top\">"
            "<th>this</th><th>is</th><th>header</th>"
            "</tr>"
            "<tr valign=\"top\">"
            "<td>1</td><td>2</td><td>3</td>"
            "</tr>"
            "</table>"
        );
    }

    {
        // Test plain old table

        std::string tableMarkdown =
            "this|is|header\n"
            "----|--|------\n"
            "1   |2 |3     \n";
        Markdown::TableElement table(tableMarkdown);

        REQUIRE(table.getHtml() == 
            "<table>"
            "<tr>"
            "<th>this</th><th>is</th><th>header</th>"
            "</tr>"
            "<tr>"
            "<td>1</td><td>2</td><td>3</td>"
            "</tr>"
            "</table>"
        );
    }
}