#ifndef _h_cppmarkdownblankelement
#define _h_cppmarkdownblankelement

#include "cppmarkdowncommon.h"
#include "document.h"

#include <vector>

namespace Markdown
{
    class BlankElement : public Element
    {
    public:
        BlankElement(const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

    protected:
        std::string text;
    };

    std::shared_ptr<BlankElement> toBlankElement(const Element& element);
}

#endif