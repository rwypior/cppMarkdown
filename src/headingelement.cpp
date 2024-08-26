#include "headingelement.h"
#include "paragraphelement.h"

#include <cassert>

namespace Markdown
{
    HeadingElement::HeadingElement(Heading heading, const std::string& text)
        : heading(heading)
        , text(text, getDefaultStyle(heading))
    { }

	Type HeadingElement::getType() const
	{
		return Type::Heading;
	}

	ParseResult HeadingElement::parse(const std::string& line, std::shared_ptr<Element> previous)
	{
        Heading heading = parseHeadingHash(line);
        
        if (heading != Heading::Invalid)
        {
            auto headingStyle = getDefaultStyle(heading);
            this->heading = heading;
            this->text = TextEntry(getHeadingText(line), headingStyle);
            return ParseResult(ParseCode::ElementComplete);
        }

        if (previous && previous->getType() == Type::Paragraph)
        {
            heading = parseHeadingAlternate(line);

            if (heading != Heading::Invalid)
            {
                std::string text = std::static_pointer_cast<ParagraphElement>(previous)->text.getMarkdown();

                auto headingStyle = getDefaultStyle(heading);
                this->heading = heading;
                this->text = TextEntry(text, headingStyle);
                return ParseResult(ParseCode::ElementCompleteDiscardPrevious);
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

    std::shared_ptr<MarkdownStyle> HeadingElement::getDefaultStyle(Heading heading)
    {
        switch (heading)
        {
        case Heading::Heading1: return MarkdownStyle::makeHtml("<h1>", "</h1>");
        case Heading::Heading2: return MarkdownStyle::makeHtml("<h2>", "</h2>");
        case Heading::Heading3: return MarkdownStyle::makeHtml("<h3>", "</h3>");
        case Heading::Heading4: return MarkdownStyle::makeHtml("<h4>", "</h4>");
        case Heading::Heading5: return MarkdownStyle::makeHtml("<h5>", "</h5>");
        case Heading::Heading6: return MarkdownStyle::makeHtml("<h6>", "</h6>");
        }

        assert(!"Invalid heading element");
        return MarkdownStyle::makeHtml("<h1>", "</h1>");
    }

    std::string HeadingElement::getHeadingText(const std::string& line)
    {
        auto pos = line.find_first_not_of('#');
        if (pos == std::string::npos)
            return "";
        pos = line.find_first_not_of(' ', pos);
        return line.substr(pos);
    }

    HeadingElement::Heading HeadingElement::parseHeadingHash(const std::string& line)
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
            else if (c == ' ' && count > 0)
                return headings[count - 1];
            else
                return HeadingElement::Heading::Invalid;
        }

        return HeadingElement::Heading::Invalid;
    }

    HeadingElement::Heading HeadingElement::parseHeadingAlternate(const std::string& line)
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
}