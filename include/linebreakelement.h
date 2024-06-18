#ifndef _h_cppmarkdownlinebreakelement
#define _h_cppmarkdownlinebreakelement

#include "cppmarkdowncommon.h"
#include "textentry.h"

namespace Markdown
{
    class LineBreakElement : public Element
    {
    public:
        LineBreakElement() = default;

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;
    };
}

#endif