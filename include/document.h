#ifndef _h_cppmarkdowndocument
#define _h_cppmarkdowndocument

#include "cppmarkdowncommon.h"

#include <vector>

namespace Markdown
{
    class ElementContainer
    {
    public:
        using Container = std::vector<std::shared_ptr<Element>>;

    public:
        virtual ParseResult parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active = nullptr, Type mask = Type::None);
        void parse(const std::string& content, Type mask = Type::None);

        std::string dump(int indent = 0) const;

        void addElement(std::shared_ptr<Element> element);
        void addElement(std::shared_ptr<Element> element, Container::const_iterator it);

        bool empty() const;
        Container::value_type back() const;
        size_t elementsCount() const;
        Container::iterator begin();
        Container::iterator end();
        Container::const_iterator begin() const;
        Container::const_iterator end() const;

    protected:
        Container elements;

        template<typename T, typename... Args>
        ParseResult parseElement(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active = nullptr, Type mask = Type::None, Args... args)
        {
            ParseResult result;
            result.element = active ? active : std::make_shared<T>(args...);

            if ((1 << result.element->getType()) & mask)
                result.code = ParseCode::Invalid;
            else
            {
                ParseResult parseResult;
                //auto parseResult = result.element->parse(line, previous);
                if (active)
                    parseResult = result.element->supply(line, previous);
                else
                    parseResult = result.element->parse(line, previous);
                result.code = parseResult.code;
                result.flags = parseResult.flags;
            }

            return result;
        }

        void finalizeElement(std::shared_ptr<Element>& activeElement);
    };

    class Document : public ElementContainer
    {
    public:
        static Document load(const std::string& path);

        std::string getText() const;
        std::string getHtml() const;
    };
}

#endif