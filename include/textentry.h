#ifndef _h_cppmarkdowntextentry
#define _h_cppmarkdowntextentry

#include "cppmarkdowncommon.h"

#include <string>
#include <vector>

namespace Markdown
{
    struct TextEntry
    {
        struct Span
        {
            std::string text;
            MarkdownStyle style;
            size_t level;

            Span(const std::string& text, MarkdownStyle style = MarkdownStyle(), size_t level = 0)
                : text(text)
                , style(style)
                , level(level)
            { }
        };

        std::vector<Span> spans;

        TextEntry(const std::string& content, MarkdownStyle defaultStyle = MarkdownStyle());

        std::string getText() const;
        std::string getHtml() const;
        std::string getRawText() const;

        bool empty() const;
    };
}

#endif