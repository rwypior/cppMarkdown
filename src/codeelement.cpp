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

        size_t pos = 0;
        while (pos <= line.size())
        {
            if (line[pos] != ' ')
                break;
            pos++;
        }

        if (pos < 4)
        {
            if (line.front() != '\t')
                return ParseResult(ParseCode::Invalid);
            pos = 1;
        }

        if (!this->text.empty())
            this->text += '\n';
        this->text += getCodeText(line);

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
}