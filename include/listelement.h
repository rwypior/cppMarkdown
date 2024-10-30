#ifndef _h_cppmarkdownlistelement
#define _h_cppmarkdownlistelement

#include "cppmarkdowncommon.h"
#include "document.h"
#include "textentry.h"

#include <vector>

namespace Markdown
{
    class ListElement;

    class ListElementContainer : public ElementContainer
    {
    public:
        ListElementContainer(ListElement* parent = nullptr);
        virtual ParseResult parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active = nullptr, Type mask = Type::None) override;

    private:
        ListElement* parent;
    };

    class ListItem : public Element
    {
        friend class ListElement;
    public:
        ElementContainer elements;
        TextEntry text;

        ListItem(const std::string& text = "");
        ListItem(ListElement* parent);

        int getListLevel() const;

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual ParseResult supply(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual void finalize() override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;
        virtual std::string dump(int indent = 0) const override;

    protected:
        // Remove tags from paragraphs and separate paragraphs with line breaks
        void fixParagraphs();

    private:
        std::string label;
        std::string buffer;
        ListElement* parent = nullptr;
        int level = 0;
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
        std::string buffer;

        ListElement(const std::string& text = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual ParseResult supply(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual void finalize() override;
        ListItem* getLastItem() const;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;
        virtual std::string dump(int indent = 0) const override;

        static size_t findUnorderedMarker(const std::string &text);
        static ListMarker getListLevel(const std::string& line);
        static int getListIndentation(const std::string& line);
        static std::string getListText(const std::string& line);
        static std::string getListItemLineText(const std::string& line);
        static std::string getListItemText(const std::string& line);
    };
}

#endif