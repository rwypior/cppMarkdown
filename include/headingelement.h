#ifndef _h_cppmarkdownheadingelement
#define _h_cppmarkdownheadingelement

#include "cppmarkdowncommon.h"
#include "textentry.h"

namespace Markdown
{
    struct HeadingElement : Element
    {
        enum class Heading
        {
            Heading1,
            Heading2,
            Heading3,
            Heading4,
            Heading5,
            Heading6,

            Invalid
        };

        Heading heading;
        TextEntry text;

        HeadingElement(Heading heading = Heading::Heading1, const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

        static MarkdownStyle getDefaultStyle(Heading heading);
    };
}

#endif