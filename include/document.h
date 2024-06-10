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
        ParseResult parseLine(const std::string& line, std::shared_ptr<Element> previous);
        void parse(const std::string& content);

        void addElement(std::shared_ptr<Element> element);

        size_t elementsCount() const;
        Container::iterator begin();
        Container::iterator end();
        Container::const_iterator begin() const;
        Container::const_iterator end() const;

    protected:
        Container elements;
    };

    class Document : public ElementContainer
    {
    public:
        static Document load(const std::string& path);

        std::string getHtml() const;
    };
}

#endif