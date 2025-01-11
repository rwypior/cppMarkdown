#include "linebreakelement.h"

#include <cassert>
#include <algorithm>

namespace Markdown
{
    Type LineBreakElement::getType() const
    {
        return Type::LineBreak;
    }

    ParseResult LineBreakElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        if (previous && previous->getType() == Type::LineBreak)
            // Discard double line breaks
            return ParseResult(ParseCode::Invalid);
        
        if (!isAllWhitespace(line))
            return ParseResult(ParseCode::Invalid);
            
        this->skippable = isSkippable(line);

        return ParseResult(ParseCode::ElementComplete);
    }

    std::string LineBreakElement::getText() const
    {
        return "\n";
    }

    std::string LineBreakElement::getHtml() const
    {
        return "<br>";
    }

    bool LineBreakElement::isAllWhitespace(const std::string& line)
    {
        return std::all_of(line.begin(), line.end(), [](char c) { return std::isspace(c); });
    }

    bool LineBreakElement::isSkippable(const std::string& line)
    {
        if (line.size() < 2)
            return true;

        size_t spaces = line.find_first_not_of(' ');
        if (spaces >= 2)
            return false;
        return true;
    }
}
