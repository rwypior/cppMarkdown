#include "cppmarkdown.h"

namespace Markdown
{
    Type BlankElement::getType() const
    {
        return Type::Blank;
    }

    ParseResult BlankElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        char first = *line.begin();

        if (ParseResult result = parseElement<HeadingElement>(line, previous))
            return result;

        if (ParseResult result = parseElement<ParagraphElement>(line, previous))
            return result;

        return parseElement<ParagraphElement>(line, previous);
    }
}