#include "listelement.h"

#include <sstream>

namespace Markdown
{
    namespace
    {
        int getListLevel(const std::string& line)
        {
            auto pos = line.find_first_of('.');
            if (pos == std::string::npos)
                return -1;

            for (auto it = line.begin(); it != std::next(line.begin(), pos); it++)
            {
                if (!std::isdigit(*it))
                    return -1;
            }

            bool tabs = line.front() == '\t';
            char search = tabs ? '\t' : ' ';
            unsigned int count = std::count(line.begin(), std::next(line.begin(), pos), search);

            return tabs ? count : count / 4;
        }

        int getListIndentation(const std::string& line)
        {
            if (line.empty())
                return -1;

            bool tabs = line.front() == '\t';
            char search = tabs ? '\t' : ' ';
            int first = line.find_first_not_of(search);
            int count = std::count(line.begin(), std::next(line.begin(), first), search);

            return tabs ? count : count / 4;
        }

        std::string getListText(const std::string& line)
        {
            auto pos = line.find_first_of('.');
            if (pos == std::string::npos)
                return "";
            pos = line.find_first_not_of(' ', pos);
            return line.substr(pos + 1);
        }

        std::string getListItemText(const std::string& line)
        {
            if (line.empty())
                return line;

            bool tabs = line.front() == '\t';
            char search = tabs ? '\t' : ' ';
            int posNonWhitespace = line.find_first_not_of(search);

            auto pos = line.find_first_of('.');

            if (pos != std::string::npos)
            {
                std::string num = line.substr(posNonWhitespace, pos);
                if (isNumber(num.begin(), num.end()))
                    return line.substr(pos + 1);
            }

            return line.substr(posNonWhitespace + 1);
        }
    }

    // Container

    ParseResult ListElementContainer::parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active)
    {
        return parseElement<ListItem>(line, previous, active);
    }

    // List item

    ListItem::ListItem(const std::string& text)
    {
        if (!text.empty())
            this->elements.parse(getListItemText(text));
    }

    Type ListItem::getType() const
    {
        return Type::Blockquote;
    }

    ParseResult ListItem::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        int listLevel = getListLevel(line);
        if (listLevel < 0)
            return ParseResult(ParseCode::Invalid);

        this->buffer += line + "\n";
        return ParseResult(ParseCode::RequestMore);

        //int listLevel = getListLevel(line);
        //if (listLevel < 0)
        //    return ParseResult(ParseCode::Invalid);

        //if (previous && previous->getType() == Type::ListElement)
        //{
        //    auto previouslistel = std::static_pointer_cast<ListItem>(previous);
        //    if (previouslistel->getLevel() == this->getLevel())
        //    {
        //        return ParseResult(ParseCode::ElementComplete);
        //    }
        //}

        //this->text = getListText(line);
        //return ParseResult(ParseCode::ParseNextAcceptPrevious);
    }

    void ListItem::finalize()
    {
        this->elements.parse(getListItemText(this->buffer));
        this->buffer.clear();
    }

    int ListItem::getLevel() const
    {
        return this->level;
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

    ListElement::ListElement(const std::string& text)
    {
        if (!text.empty())
        {
            std::istringstream str(text);
            for (std::string line; std::getline(str, line); )
            {
                this->parse(line, nullptr);
            }

            if (!this->elements.empty())
                this->elements.back()->finalize();
        }
    }

    Type ListElement::getType() const
    {
        return Type::List;
    }

    ParseResult ListElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        int listLevel = getListLevel(line);

        if (listLevel >= 0)
        {
            if (!this->elements.empty())
                this->elements.back()->finalize();

            auto item = std::make_shared<ListItem>();
            item->buffer += line;
            
            this->elements.addElement(item);
            return ParseResult(ParseCode::ParseNext);
        }

        int indentation = getListIndentation(line);

        if (indentation >= 0)
        {
            if (this->elements.empty())
                return ParseResult(ParseCode::Invalid);

            std::static_pointer_cast<ListItem>(this->elements.back())->buffer += line;
            return ParseResult(ParseCode::ParseNext);
        }

        if (!this->elements.empty())
            this->elements.back()->finalize();

        return ParseResult(ParseCode::Invalid);
    }

    void ListElement::finalize()
    {
        //if (!this->buffer.empty())
        //{
        //    this->elements.parse(this->buffer);
        //    this->buffer.clear();
        //}
    }

    std::string ListElement::getText() const
    {
        std::string str;
        unsigned int number = 1;
        for (const auto& element : this->elements)
        {
            str += std::to_string(number++) + ". " + element->getText() + "\n";
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