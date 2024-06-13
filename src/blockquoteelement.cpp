#include "blockquoteelement.h"

#include <sstream>

namespace Markdown
{
    BlockquoteElement::BlockquoteElement(const std::string& text)
    {
        if (!text.empty())
            this->elements.parse(text);
    }

    Type BlockquoteElement::getType() const
    {
        return Type::Blockquote;
    }

    ParseResult BlockquoteElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        int level = getBlockquoteLevel(line);

        if (level == -1)
            return ParseResult(ParseCode::Invalid);

        auto el = std::make_shared<BlockquoteElement>();
        el->level = level;

        this->buffer += line + "\n";
        return ParseResult(ParseCode::RequestMore, el);
    }

    void BlockquoteElement::finalize()
    {
        this->elements.parse(getEligibleText(this->buffer));
        this->buffer.clear();
    }

    std::string BlockquoteElement::getText() const
    {
        std::string str;
        for (const auto& element : this->elements)
        {
            str += element->getText() + "\n";
        }
        if (!str.empty())
            str.pop_back();
        return str;
    }

    std::string BlockquoteElement::getHtml() const
    {
        std::string html = "<blockquote>";
        for (const auto& element : this->elements)
        {
            html += element->getHtml();
        }
        html += "</blockquote>";
        return html;
    }

    int BlockquoteElement::getBlockquoteLevel(const std::string& line)
    {
        auto pos = line.find_first_not_of(' ');
        if (pos != std::string::npos && line[pos] == '>')
        {
            auto posEnd = line.find_first_not_of('>', pos);
            return posEnd - pos;
        }
        return -1;
    }

    std::string BlockquoteElement::getBlockquoteText(const std::string& line)
    {
        auto pos = line.find_first_not_of('>');
        if (pos == std::string::npos)
            return "";
        pos = line.find_first_not_of(' ', pos);
        return line.substr(pos);
    }

    std::string BlockquoteElement::getEligibleText(const std::string& text)
    {
        std::string result;
        std::istringstream str(text);
        for (std::string line; std::getline(str, line); )
        {
            int linelevel = getBlockquoteLevel(line);
            if (linelevel != -1)
            {
                result += std::string(linelevel - 1, '>') + getBlockquoteText(line) + "\n";
            }
        }
        return result;
    }
}