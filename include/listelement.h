#ifndef _h_cppmarkdownlistelement
#define _h_cppmarkdownlistelement

#include "cppmarkdowncommon.h"
#include "document.h"

#include <vector>

namespace Markdown
{
    class ListElementContainer : public ElementContainer
    {
    public:
        virtual ParseResult parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active = nullptr) override;
    };

    class ListItem : public Element
    {
        friend class ListElement;
    public:
        ElementContainer elements;

        ListItem(const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual void finalize() override;

        int getLevel() const;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

    private:
        int level = 0;
        std::string text;
        std::string buffer;
    };

    class ListElement : public Element
    {
    public:
        ListElementContainer elements;

        ListElement(const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual void finalize() override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

    private:
        int level = 0;
        //std::string buffer;
    };
}

#endif