#include "cppmarkdown.h"

namespace Markdown
{
    std::string getHeadingText(const std::string& line)
    {
        auto pos = line.find_first_not_of('#');
        if (pos == std::string::npos)
            return "";
        pos = line.find_first_not_of(' ', pos);
        return line.substr(pos);
    }

    HeadingElement::Heading parseHeadingHash(const std::string &line)
    {
        std::vector<HeadingElement::Heading> headings{ 
            HeadingElement::Heading::Heading1,
            HeadingElement::Heading::Heading2,
            HeadingElement::Heading::Heading3,
            HeadingElement::Heading::Heading4,
            HeadingElement::Heading::Heading5,
            HeadingElement::Heading::Heading6,
        };
        
        unsigned int count = 0;
        for (char c : line)
        {
            if (c == '#')
            {
                count++;
                if (count >= headings.size())
                    return HeadingElement::Heading::Invalid;
            }
            else if (c == ' ')
                return headings[count - 1];
            else
                return HeadingElement::Heading::Invalid;
        }

        return HeadingElement::Heading::Invalid;
    }

    HeadingElement::Heading parseHeadingAlternate(const std::string &line)
    {
        if (line.empty())
            return HeadingElement::Heading::Invalid;

        size_t p = line.find_first_not_of('=');
        if (line.front() == '=' && p == std::string::npos)
        {
            return HeadingElement::Heading::Heading1;
        }

        p = line.find_first_not_of('-');
        if (line.front() == '-' && p == std::string::npos)
        {
            return HeadingElement::Heading::Heading2;
        }

        return HeadingElement::Heading::Invalid;
    }

	Type HeadingElement::getType() const
	{
		return Type::Heading;
	}

	ParseResult HeadingElement::parse(const std::string& line, std::shared_ptr<Element> previous)
	{
        Heading heading = parseHeadingHash(line);
        
        if (heading != Heading::Invalid)
        {
            return ParseResult(
                ParseCode::ElementComplete,
                std::make_shared<HeadingElement>(heading, getHeadingText(line))
            );
        }

        if (previous && previous->getType() == Type::Paragraph)
        {
            heading = parseHeadingAlternate(line);

            if (heading != Heading::Invalid)
            {
                std::string text = std::static_pointer_cast<ParagraphElement>(previous)->text.getRawText();

                return ParseResult(
                    ParseCode::ElementCompleteDiscardPrevious,
                    std::make_shared<HeadingElement>(heading, text)
                );
            }
        }

		return ParseResult(ParseCode::Invalid);
	}

    std::string HeadingElement::getText() const
    {
        return this->text.getText();
    }

    std::string HeadingElement::getHtml() const
    {
        return this->text.getHtml();
    }
}