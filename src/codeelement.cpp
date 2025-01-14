#include "codeelement.h"

#include <cassert>

namespace Markdown
{
    CodeElement::CodeElement(const std::string& text)
        : text(text)
    { }

    Type CodeElement::getType() const
    {
        return Type::Code;
    }

    ParseResult CodeElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        if (line.empty())
            return ParseResult(ParseCode::Invalid);
        
        if (previous && previous->getType() != Type::LineBreak)
            // Must have line break before code block
            return ParseResult(ParseCode::Invalid);

        size_t pos = getCodeLevel(line);

        if (pos == 0)
        {
            // Not started with tabs
            return ParseResult(ParseCode::Invalid);
        }

        std::string code = getCodeText(line);
        this->text += code;

        // Previous element must be a line break
        // Since this is block-type element anyways - the line break is redundant - remove it
        return ParseResult(ParseCode::RequestMore, ParseFlags::ErasePrevious);
    }

    ParseResult CodeElement::supply(const std::string& line, std::shared_ptr<Element>)
    {
        if (line.empty())
            return ParseResult(ParseCode::ElementCompleteParseNext);

        size_t pos = getCodeLevel(line);
        if (pos == 0)
            return ParseResult(ParseCode::ElementCompleteParseNext);

        std::string code = getCodeText(line);
        this->text += "\n" + code;

        return ParseResult(ParseCode::RequestMore);
    }

    std::string CodeElement::getText() const
    {
        return this->text;
    }

    std::string CodeElement::getHtml() const
    {
        return "<code>" + this->text + "</code>";
    }

    std::string CodeElement::getMarkdown() const
    {
        auto splitted = split(this->getText(), '\n');
        std::string result;
        for (const auto& line : splitted)
        {
            result += "    " + line + "\n";
        }
        result.pop_back();
        return result;
    }

    std::string CodeElement::getCodeText(const std::string& line)
    {
        if (line.empty())
            return line;

        auto chr = 0;
        if (line.front() == '\t')
            chr = '\t';
        else
            chr = ' ';
        auto pos = line.find_first_not_of(chr);

        if (pos == std::string::npos)
            return "";

        return line.substr(pos);
    }

    unsigned int CodeElement::getCodeLevel(const std::string& line)
    {
        size_t pos = 0;
        while (pos <= line.size())
        {
            if (line[pos] != ' ')
                break;
            pos++;
        }

        if (pos)
            return pos / 4;

        while (pos <= line.size())
        {
            if (line[pos] != '\t')
                break;
            pos++;
        }

        return pos;
    }
}
