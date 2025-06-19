#ifndef _h_cppmarkdownheadingelement
#define _h_cppmarkdownheadingelement

#include "cppmarkdown/cppmarkdowncommon.h"
#include "cppmarkdown/textentry.h"

namespace Markdown
{
    class HeadingElement : public Element
    {
    public:
        enum class Heading
        {
            Heading1 = 1,
            Heading2 = 2,
            Heading3 = 3,
            Heading4 = 4,
            Heading5 = 5,
            Heading6 = 6,

            Invalid
        };

        Heading heading;
        TextEntry text;

        HeadingElement(Heading heading = Heading::Heading1, const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;
        virtual std::string getMarkdown() const override;

        static std::shared_ptr<MarkdownStyle> getDefaultStyle(Heading heading);
        static std::string getHeadingText(const std::string& line);
        static Heading parseHeadingHash(const std::string& line);
        static Heading parseHeadingAlternate(const std::string& line);
    };
}

#endif