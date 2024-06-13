#ifndef _h_cppmarkdownblockquoteelement
#define _h_cppmarkdownblockquoteelement

#include "cppmarkdowncommon.h"
#include "document.h"

#include <vector>

namespace Markdown
{
    class BlockquoteElement : public Element
    {
    public:
        ElementContainer elements;

        BlockquoteElement(const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual void finalize() override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

        static int getBlockquoteLevel(const std::string& line);
        static std::string getBlockquoteText(const std::string& line);
        static std::string getEligibleText(const std::string& text);

    private:
        int level = 0;
        std::string buffer;
    };
}

#endif