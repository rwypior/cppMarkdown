#ifndef _h_cppmarkdownblankelement
#define _h_cppmarkdownblankelement

#include "cppmarkdown/cppmarkdowncommon.h"
#include "cppmarkdown/document.h"

#include <vector>

namespace Markdown
{
    // An element without any special parsing - only raw text
    class BlankElement : public Element
    {
    public:
        BlankElement(const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;
        virtual std::string getMarkdown() const override { return ""; }

    protected:
        std::string text;
    };

    // Convert any element to blank element with source element's text
    std::shared_ptr<BlankElement> toBlankElement(const Element& element);

    // Insert line break element between any consecutive blank elements
    void prependBlankElementsWithNewlines(ElementContainer& container, ElementContainer::Container::iterator pos, const Element* prev, const Element* next);
}

#endif