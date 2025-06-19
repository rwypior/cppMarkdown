#include "cppmarkdown/linebreakelement.h"
#include "cppmarkdown/html.h"

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
        if (!isAllWhitespace(line))
            return ParseResult(ParseCode::Invalid);

        if (previous && previous->getType() == Type::LineBreak)
        {
            // Discard double line breaks
            
            // Make one of consecutive line breaks not to be deleted in finalize step
            // TODO - Not sure if this is correct for Markdown syntax, need to check
            std::static_pointer_cast<LineBreakElement>(previous)->skippable = false;
            return ParseResult(ParseCode::Invalid);
        }
            
        this->skippable = isSkippable(line);

        return ParseResult(ParseCode::ElementComplete);
    }

    std::string LineBreakElement::getText() const
    {
        return "\n";
    }

    std::string LineBreakElement::getHtml() const
    {
        auto& tag = HtmlProvider::get().getLineBreak();
        return tag.first + tag.second;
    }

    std::string LineBreakElement::getMarkdown() const
    {
        return "\n";
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
