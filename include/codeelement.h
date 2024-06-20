#ifndef _h_cppmarkdowncodeelement
#define _h_cppmarkdowncodeelement

#include "cppmarkdowncommon.h"
#include "textentry.h"

namespace Markdown
{
    class CodeElement : public Element
    {
    public:
        std::string text;

        CodeElement(const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

        static std::string getCodeText(const std::string& line);
    };
}

#endif