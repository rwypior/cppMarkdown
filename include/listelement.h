#ifndef _h_cppmarkdownlistelement
#define _h_cppmarkdownlistelement

#include "cppmarkdowncommon.h"
#include "document.h"
#include "textentry.h"

#include <vector>

namespace Markdown
{
    class ListElement;

    class ListItem : public Element, public SubelementUnpacker, public ElementContainer
    {
        friend class ListElement;
    public:
        //ElementContainer elements;
        TextEntry text;

        ListItem(const std::string& text = "");
        ListItem(ListElement* parent);

        int getListLevel() const;
        bool sublistFirst() const;

        std::shared_ptr<Element> getLastItem() const;

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual ParseResult supply(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual void finalize() override;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;
        virtual std::string dump(int indent = 0) const override;

    protected:
        virtual ElementContainer& getContainer() override;
        virtual TextEntry& getText() override;

    private:
        std::string label;
        std::string buffer;
        ListElement* parent = nullptr;
        int level = 0;
    };

    //class ListElement : public Element, public SubelementParser
    class ListElement : public Element, public ElementContainer
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
            int level;
            ListType type;

            ListMarker(int level = -1, ListType type = ListType::Invalid)
                : level(level)
                , type(type)
            { }

            operator bool() const
            {
                return type != ListType::Invalid;
            }
        };

    public:
        using ElementContainer::size;

        ListType listType = ListType::Invalid;
        std::string buffer;

        ListElement(const std::string& text = "");

        virtual void addElement(std::shared_ptr<Element> element) override;
        virtual void addElement(std::shared_ptr<Element> element, Container::const_iterator it) override;

        virtual ParseResult parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active = nullptr, Type mask = Type::None) override;

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual ParseResult supply(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual void finalize() override;
        std::shared_ptr<ListItem> getLastItem() const;

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;
        virtual std::string dump(int indent = 0) const override;

        static size_t findUnorderedMarker(const std::string &text);
        static size_t findOrderedMarker(const std::string &text);
        static size_t findMarker(const std::string &text);
        static ListMarker getListLevel(const std::string& line);
        static int getListIndentation(const std::string& line);
        static std::string getListText(const std::string& line);
        static std::string getListTextWithMarker(const std::string& line);
        static std::string getListItemLineText(const std::string& line);
        static std::string getListItemText(const std::string& line);
    };
}

#endif