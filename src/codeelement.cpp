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

            if (this->text.empty())
            {
                // Had no text before this iteration - invalid for code block
                return ParseResult(ParseCode::Invalid);
            }
            else
            {
                // Had some text before - finish the element
                //return ParseResult(ParseCode::ElementComplete);
                return ParseResult(ParseCode::ElementCompleteParseNext);
            }
        }

        std::string code = getCodeText(line);
        if (!this->text.empty())
            this->text += '\n';
        this->text += code;

        //return ParseResult(ParseCode::RequestMore);

        // Previous element must be a line break
        // Since this is block-type element anyways - the line break is redundant - remove it
        return ParseResult(ParseCode::RequestMore, ParseFlags::ErasePrevious);
    }

    std::string CodeElement::getText() const
    {
        return this->text;
    }

    std::string CodeElement::getHtml() const
    {
        return "<code>" + this->text + "</code>";
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