#ifndef _h_cppmarkdownlinebreakelement
#define _h_cppmarkdownlinebreakelement

#include "cppmarkdowncommon.h"
#include "textentry.h"

namespace Markdown
{
    class LineBreakElement : public Element
    {
    public:
        // Line breaks are skippable by default - meaning duplicate linebreaks will be ommitted
        // If a line consists of only two or more whitespaces - it's always inserted
        bool skippable = true;

        LineBreakElement() = default;

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

        static bool isAllWhitespace(const std::string& line);
        static bool isSkippable(const std::string& line);
    };
}

#endif