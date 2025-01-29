#ifndef _h_cppmarkdownlineelement
#define _h_cppmarkdownlineelement

#include "cppmarkdown/cppmarkdowncommon.h"
#include "cppmarkdown/textentry.h"

namespace Markdown
{
    class LineElement : public Element
    {
    public:
        unsigned int textLineLength;
        char textLineCharacter;

        LineElement(unsigned int textLineLength = 3, char textLineCharacter = '-');

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;
        virtual std::string getMarkdown() const override;
    };
}

#endif