#include "cppmarkdown/headingelement.h"
#include "cppmarkdown/paragraphelement.h"
#include "cppmarkdown/html.h"

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
                return ParseResult(ParseCode::ReplacePrevious);
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

    std::string HeadingElement::getMarkdown() const
    {
        return std::string(static_cast<int>(this->heading) + 1, '#') + this->getText();
    }

    std::shared_ptr<MarkdownStyle> HeadingElement::getDefaultStyle(Heading heading)
    {
        if (heading == Heading::Invalid)
        {
            assert(!"Invalid heading");
            heading = Heading::Heading1;
        }

        auto& tag = HtmlProvider::get().getHeading(static_cast<int>(heading));
        return MarkdownStyle::makeHtml(tag.first, tag.second);
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
