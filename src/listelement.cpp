#include "listelement.h"

#include <sstream>

namespace Markdown
{
    // Container

    ParseResult ListElementContainer::parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active)
    {
        return parseElement<ListItem>(line, previous, active);
    }

    // List item

    ListItem::ListItem(const std::string& text)
    {
        if (!text.empty())
            this->elements.parse(text);
    }

    Type ListItem::getType() const
    {
        return Type::Blockquote;
    }

    ParseResult ListItem::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        return ParseResult(ParseCode::Invalid);
    }

    void ListItem::finalize()
    {
        //this->elements.parse(getEligibleText(this->buffer));
        this->buffer.clear();
    }

    std::string ListItem::getText() const
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

    std::string ListItem::getHtml() const
    {
        std::string html = "<ol>";
        for (const auto& element : this->elements)
        {
            html += element->getHtml();
        }
        html += "</ol>";
        return html;
    }

    // List element

    bool isListElement(const std::string& line)
    {
        auto pos = line.find_first_of('.');
        if (pos == std::string::npos)
            return false;

        for (auto it = line.begin(); it != std::next(line.begin(), pos); it++)
        {
            if (!std::isdigit(*it))
                return false;
        }

        return true;
    }

    std::string getBlockquoteText(const std::string& line)
    {
        auto pos = line.find_first_of('.');
        if (pos == std::string::npos)
            return "";
        return line.substr(pos + 1);
    }

    //std::string getEligibleText(const std::string& text)
    //{
    //    std::string result;
    //    std::istringstream str(text);
    //    for (std::string line; std::getline(str, line); )
    //    {
    //        int linelevel = getBlockquoteLevel(line);
    //        if (linelevel != -1)
    //        {
    //            result += std::string(linelevel - 1, '>') + getBlockquoteText(line) + "\n";
    //        }
    //    }
    //    return result;
    //}

    ListElement::ListElement(const std::string& text)
    {
        if (!text.empty())
            this->elements.parse(text);
    }

    Type ListElement::getType() const
    {
        return Type::Blockquote;
    }

    ParseResult ListElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        bool isList = isListElement(line);

        if (!isList)
            return ParseResult(ParseCode::Invalid);

        auto el = std::make_shared<ListElement>();

        this->buffer += line + "\n";
        return ParseResult(ParseCode::RequestMore, el);
    }

    void ListElement::finalize()
    {
        if (!this->buffer.empty())
        {
            this->elements.parse(this->buffer);
            this->buffer.clear();
        }
    }

    std::string ListElement::getText() const
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

    std::string ListElement::getHtml() const
    {
        std::string html = "<ol>";
        for (const auto& element : this->elements)
        {
            html += element->getHtml();
        }
        html += "</ol>";
        return html;
    }
}