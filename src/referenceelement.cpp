#include "cppmarkdown/referenceelement.h"

#include <sstream>

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
		// Find ID
		size_t begin = line.find_first_of('[');

		size_t end = begin;
		bool found = false;
		do
		{
			end = line.find_first_of(']', end);
			if (end == std::string::npos || line[end - 1] == '\\')
				continue;
			found = true;
		} while (end != std::string::npos && !found);

		if (end == std::string::npos)
			return ParseResult(ParseCode::Invalid);

		if (end + 1 == line.length())
			return ParseResult(ParseCode::Invalid);

        // Obligatory colon
        if (line.at(end + 1) != ':')
			return ParseResult(ParseCode::Invalid);

        // Find URL
		size_t beginUrl = end + 2;

        while (std::isspace(line.at(beginUrl)))
        {
			beginUrl++;
            if (beginUrl >= line.length())
                return ParseResult(ParseCode::Invalid);
        }

		size_t endUrl = beginUrl + 1;

        while (!std::isspace(line.at(endUrl)) || line.at(endUrl - 1) == '\\')
        {
			endUrl++;
            if (endUrl >= line.length())
            {
                endUrl--;
                break;
            }
        }

        // Optionally find title
        size_t titleBegin = std::string::npos;
        size_t titleEnd = std::string::npos;
        if (endUrl + 1 <= line.length())
        {
            titleBegin = line.find_first_of("\"'(", endUrl);
            const std::unordered_map<char, char> titleCharMap{
                {'"', '"'},
                {'\'', '\''},
                {'(', ')'}
            };
            if (titleBegin != std::string::npos)
            {
                char titleChar = line.at(titleBegin);
                char endingChar = titleCharMap.at(titleChar);

                titleEnd = titleBegin + 1;
                while (line.at(titleEnd) != endingChar || line.at(titleEnd - 1) == '\\')
                {
                    titleEnd++;
                    if (titleEnd >= line.length())
                    {
                        titleEnd--;
                        break;
                    }
                }
            }
        }

        // Assemble
        this->reference.id = line.substr(begin + 1, end - begin - 1);
        this->reference.value = trimmed(line.substr(beginUrl, endUrl - beginUrl + 1));

        this->reference.value.erase(std::remove(this->reference.value.begin(), this->reference.value.end(), '\\'), this->reference.value.end());

        if (titleBegin != std::string::npos && titleEnd != std::string::npos)
        {
            this->reference.title = trimmed(line.substr(titleBegin + 1, titleEnd - titleBegin - 1));
            this->reference.title.erase(std::remove(this->reference.title.begin(), this->reference.title.end(), '\\'), this->reference.title.end());
        }

        if (ReferenceManager* man = ReferenceManager::get())
            man->registerReference(this->reference);

        return ParseResult(ParseCode::ElementComplete);
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