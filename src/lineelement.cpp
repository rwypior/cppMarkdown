#include "lineelement.h"
#include "paragraphelement.h"

#include <cassert>

namespace Markdown
{
    LineElement::LineElement(unsigned int textLineLength, char textLineCharacter)
        : textLineLength(textLineLength)
        , textLineCharacter(textLineCharacter)
    { }

    Type LineElement::getType() const
    {
        return Type::Line;
    }

    ParseResult LineElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        // Can only have line if previous element was not paragraph
        // Or the paragraph has an empty line following it
        if (previous && previous->getType() == Type::Paragraph && !std::static_pointer_cast<ParagraphElement>(previous)->getText().empty())
            return ParseResult(ParseCode::Invalid);

        std::vector<char> linechars = {'*', '-', '_'};
        std::string txttrimmed = trimmed(line);

        bool good = false;
        for (char c : linechars)
        {
            if (txttrimmed.find_first_not_of(c) == std::string::npos)
            {
                good = true;
                break;
            }
        }

        if (good)
            return ParseResult(ParseCode::ElementComplete);

        return ParseResult(ParseCode::Invalid);
    }

    std::string LineElement::getText() const
    {
        return std::string(this->textLineLength, this->textLineCharacter);
    }

    std::string LineElement::getHtml() const
    {
        return "<hr>";
    }
}