#ifndef _h_cppmarkdowntextentry
#define _h_cppmarkdowntextentry

#include "cppmarkdowncommon.h"

#include <string>
#include <vector>

namespace Markdown
{
    class TextEntry
    {
    public:
        enum class HtmlOptions
        {
            Normal = 0x00,
            SkipDefaultTags = 0x01
        };

    public:
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
        std::string getHtml(HtmlOptions flags = HtmlOptions::Normal) const;
        std::string getRawText() const;

        bool empty() const;
    };

    DEFINE_BITFIELD(TextEntry::HtmlOptions);
}

#endif