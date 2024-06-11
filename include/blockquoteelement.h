#ifndef _h_cppmarkdownblockquoteelement
#define _h_cppmarkdownblockquoteelement

#include "cppmarkdowncommon.h"
#include "document.h"

#include <vector>

namespace Markdown
{
    struct BlockquoteElement : Element
    {
        ElementContainer elements;

        BlockquoteElement(const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual void finalize() override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

    private:
        int level = 0;
        std::string buffer;
    };
}

#endif