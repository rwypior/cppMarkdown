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
        ElementContainer elements;

        BlankElement(const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual void finalize() override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;
    };
}

#endif