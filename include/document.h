#ifndef _h_cppmarkdowndocument
#define _h_cppmarkdowndocument

#include "cppmarkdowncommon.h"

#include <vector>
#include <functional>

namespace Markdown
{
    class ElementContainer
    {
    public:
        using Container = std::vector<std::shared_ptr<Element>>;

    public:
        virtual ParseResult parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active = nullptr, Type mask = Type::None);
        virtual void parse(const std::string& content, Type mask = Type::None);

        std::string dump(int indent = 0) const;

        virtual void addElement(std::shared_ptr<Element> element);
        virtual void addElement(std::shared_ptr<Element> element, Container::const_iterator it);
        virtual void iterate(std::function<void(ElementContainer& container, Container::iterator, const Element*, const Element*)> pred);

        virtual void eraseElement(Container::const_iterator it);

        virtual void replaceElement(Container::iterator it, std::shared_ptr<Element> replacement);
        virtual void replaceElements(std::function<bool(const Element&)> pred, std::function<std::shared_ptr<Element>(const Element&)> replacementPred);
        virtual void replaceElements(Type type, std::function<std::shared_ptr<Element>(const Element&)> replacementPred);

        bool empty() const;
        void clear();
        size_t size() const;
        std::shared_ptr<Element> at(size_t index) const;
        std::shared_ptr<Element> take(size_t index);
        Container::value_type front() const;
        Container::value_type back() const;
        size_t elementsCount() const;
        Container::iterator begin();
        Container::iterator end();
        Container::const_iterator begin() const;
        Container::const_iterator end() const;

        template<typename T, typename... Args>
        static ParseResult parseElement(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active = nullptr, Type mask = Type::None, Args... args)
        {
            ParseResult result;
            result.element = active ? active : std::make_shared<T>(args...);

            if ((1 << result.element->getType()) & mask)
                result.code = ParseCode::Invalid;
            else
            {
                ParseResult parseResult;
                if (active)
                    parseResult = result.element->supply(line, previous);
                else
                    parseResult = result.element->parse(line, previous);
                result.code = parseResult.code;
                result.flags = parseResult.flags;
            }

            return result;
        }

    protected:
        Container elements;

        void finalizeElement(std::shared_ptr<Element>& activeElement);
    };

    class Document : public ElementContainer
    {
    public:
        static Document load(const std::string& path);
        virtual void parse(const std::string& content, Type mask = Type::None) override;

        std::string getText() const;
        std::string getHtml() const;

    private:
        ReferenceManager referenceManager;
    };

    class SubelementContainer
    {
    protected:
        virtual ElementContainer& getContainer() = 0;
    };

    // Provide functionality for parseSubelements for Element classes
    class SubelementParser
    {
    public:
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) = 0;
        virtual ParseResult supply(const std::string& /*line*/, std::shared_ptr<Element> /*previous*/) { return ParseResult(ParseCode::Invalid); }
        virtual void finalize() {};

    protected:
        void parseSubelements(const std::string& source);
    };

    // Provide functionality for element to text unpacking
    class SubelementUnpacker : public SubelementContainer
    {
    protected:
        void unpackTo(SubelementUnpacker& target);
        void unpackToText(Type mask = Type::None);
        void unpackElement(ElementContainer::Container::const_iterator it);

        virtual TextEntry& getText() = 0;
    };
}

#endif
