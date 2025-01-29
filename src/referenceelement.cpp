#include "cppmarkdown/referenceelement.h"

#include <sstream>
#include <regex>

namespace Markdown
{
    ReferenceElement::ReferenceElement(const std::string& line)
    {
        ParseResult res = this->parse(line, nullptr);
        if (res.code == ParseCode::ElementComplete)
            this->finalize();
    }

    ReferenceElement::ReferenceElement(const std::string& id, const std::string& value, const std::string &title)
        : reference(id, value, title)
    {
    }

    Type ReferenceElement::getType() const
    {
        return Type::Reference;
    }

    ParseResult ReferenceElement::parse(const std::string& line, std::shared_ptr<Element>)
    {
        std::regex regex(R"r(\[(.*?)\]:(.+?)(?: +((\"|\')(.+)(\4)))?$)r");
        std::smatch matches;
        if (std::regex_match(line.begin(), line.end(), matches, regex))
        {
            this->reference.id = std::next(matches.begin(), 1)->str();
            this->reference.value = trimmed(std::next(matches.begin(), 2)->str());
            this->reference.title = trimmed(std::next(matches.begin(), 5)->str());

            if (ReferenceManager* man = ReferenceManager::get())
                man->registerReference(this->reference);

            return ParseResult(ParseCode::ElementComplete);
        }

        return ParseResult(ParseCode::Invalid);
    }

    const Reference& ReferenceElement::getReference() const
    {
        return this->reference;
    }

    const std::string& ReferenceElement::getId() const
    {
        return this->reference.id;
    }

    const std::string& ReferenceElement::getValue() const
    {
        return this->reference.value;
    }

    const std::string& ReferenceElement::getTitle() const
    {
        return this->reference.title;
    }
}