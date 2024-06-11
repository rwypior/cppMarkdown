#include "blockquoteelement.h"

#include <cassert>
#include <sstream>

namespace Markdown
{
    int getBlockquoteLevel(const std::string& line)
    {
        auto pos = line.find_first_not_of(' ');
        if (pos != std::string::npos && line[pos] == '>')
        {
            auto posEnd = line.find_first_not_of('>', pos);
            return posEnd - pos;
        }
        return -1;
    }

    std::string getBlockquoteText(const std::string& line)
    {
        auto pos = line.find_first_not_of('>');
        if (pos == std::string::npos)
            return "";
        pos = line.find_first_not_of(' ', pos);
        return line.substr(pos);
    }

    std::string getEligibleText(const std::string& text)
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

    BlockquoteElement::BlockquoteElement(const std::string& text)
    {
        /*if (!text.empty())
            this->elements.parse(getEligibleText(text));*/
        if (!text.empty())
            this->elements.parse(text);
    }

    Type BlockquoteElement::getType() const
    {
        return Type::Blockquote;
    }

    // TODO:
    // Change parse methods to static functions?
    // Add 'level' and 'textBuffer' variables to blockquote and keep track of them
    // When level is above zero - keep buffering, when it switches back to zero, then parse the whole buffer
    // and return new shared_ptr
    // Use previous element to gather the previous buffer

    ParseResult BlockquoteElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        int level = getBlockquoteLevel(line);

        if (level == -1)
            return ParseResult(ParseCode::Invalid);

        auto el = std::make_shared<BlockquoteElement>();
        el->level = level;
        //std::string blockquoteLine = getBlockquoteText(line, level);
        std::string blockquoteLine = getEligibleText(line);
        this->buffer += line + "\n";
        return ParseResult(ParseCode::RequestMore, el);

        //if (previous && previous->getType() == Type::Blockquote)
        //{
        //    auto previousBlockquote = std::static_pointer_cast<BlockquoteElement>(previous);
        //    auto el = std::make_shared<BlockquoteElement>();
        //    std::string blockquoteLine = getBlockquoteText(line);
        //    el->buffer = previousBlockquote->buffer + line + "\n";
        //    return ParseResult(ParseCode::RequestMore, el);
        //}

        //this->elements.parse(line);
        //return ParseResult(ParseCode::ElementComplete, std::make_shared<BlockquoteElement>(previousBlockquote->buffer + line + "\n"));

        this->elements.parse(this->buffer);
        return ParseResult(ParseCode::ElementComplete);

        //int level = getBlockquoteLevel(line);

        //if (level == -1)
        //    return ParseResult(ParseCode::Invalid);

        //if (previous && previous->getType() == Type::Blockquote)
        //{
        //    auto previousBlockquote = std::static_pointer_cast<BlockquoteElement>(previous);
        //    return ParseResult(ParseCode::RequestMore, std::make_shared<BlockquoteElement>(previousBlockquote->buffer + line + "\n"));
        //}

        //this->elements.parse(line);
        ////return ParseResult(ParseCode::ElementComplete, std::make_shared<BlockquoteElement>(previousBlockquote->buffer + line + "\n"));

        //return ParseResult(ParseCode::ElementComplete, std::make_shared<BlockquoteElement>(line));
    }

    void BlockquoteElement::finalize()
    {
        this->elements.parse(getEligibleText(this->buffer));
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
}