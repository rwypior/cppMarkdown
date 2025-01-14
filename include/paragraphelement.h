#ifndef _h_cppmarkdownparagraphelement
#define _h_cppmarkdownparagraphelement

#include "cppmarkdowncommon.h"
#include "textentry.h"

namespace Markdown
{
    class ParagraphElement : public Element
    {
    public:
        TextEntry text;

        ParagraphElement(const std::string& content = "");

        std::shared_ptr<MarkdownStyle> getParagraphStyle() const;

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;
        virtual std::string getMarkdown() const override;
    };
}

#endif