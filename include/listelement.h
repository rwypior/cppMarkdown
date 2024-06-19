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

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

    protected:
        // Remove tags from paragraphs and separate paragraphs with line breaks
        void fixParagraphs();

    private:
        std::string text;
        std::string buffer;
    };

    class ListElement : public Element
    {
    public:
        enum class ListType
        {
            Invalid,
            Ordered,
            Unordered
        };

        struct ListMarker
        {
            int level = -1;
            ListType type = ListType::Invalid;
        };

    public:
        ListElementContainer elements;
        ListType listType;

        ListElement(const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual void finalize() override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

        static size_t findUnorderedMarker(const std::string &text);
        static ListMarker getListLevel(const std::string& line);
        static int getListIndentation(const std::string& line);
        static std::string getListText(const std::string& line);
        static std::string getListItemLineText(const std::string& line);
        static std::string getListItemText(const std::string& line);
    };
}

#endif