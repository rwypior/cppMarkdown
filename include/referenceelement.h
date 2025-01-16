#ifndef _h_cppmarkdownreferenceelement
#define _h_cppmarkdownreferenceelement

#include "cppmarkdowncommon.h"
#include "document.h"

#include <vector>

namespace Markdown
{
    // Pseudo element used to parse reference for links and images
    class ReferenceElement : public Element
    {
    public:
        ReferenceElement(const std::string& line = "");
        ReferenceElement(const std::string& id, const std::string& value, const std::string& title = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;

        const Reference& getReference() const;
        const std::string& getId() const;
        const std::string& getValue() const;
        const std::string& getTitle() const;

        virtual std::string getText() const override { return ""; }
        virtual std::string getHtml() const override { return ""; }
        virtual std::string getMarkdown() const override { return ""; }

    protected:
        Reference reference;
    };
}

#endif