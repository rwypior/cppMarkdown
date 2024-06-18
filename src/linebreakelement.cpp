#include "linebreakelement.h"

#include <cassert>

namespace Markdown
{
    Type LineBreakElement::getType() const
    {
        return Type::LineBreak;
    }

    ParseResult LineBreakElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        if (line.find_first_of('\n') != std::string::npos)
            return ParseResult(ParseCode::ElementComplete);

        return ParseResult(ParseCode::Invalid);
    }

    std::string LineBreakElement::getText() const
    {
        return "\n";
    }

    std::string LineBreakElement::getHtml() const
    {
        return "<br>";
    }
}