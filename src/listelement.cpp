#include "listelement.h"
#include "linebreakelement.h"

#include <sstream>
#include <cassert>

namespace Markdown
{
    // Container

    ParseResult ListElementContainer::parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active, Type mask)
    {
        return parseElement<ListItem>(line, previous, active);
    }

    // List item

    ListItem::ListItem(const std::string& text)
    {
        if (!text.empty())
            this->elements.parse(ListElement::getListItemText(text));
    }

    Type ListItem::getType() const
    {
        return Type::ListItem;
    }

    ParseResult ListItem::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        ListElement::ListMarker marker = ListElement::getListLevel(line);
        int listLevel = marker.level;
        if (listLevel < 0)
            return ParseResult(ParseCode::Invalid);

        this->buffer += line + "\n";
        return ParseResult(ParseCode::RequestMore);
    }

    void ListItem::finalize()
    {
        std::string text = ListElement::getListItemText(this->buffer);
        this->elements.parse(text, 1 << Type::Paragraph);

        if (this->elements.empty())
        {
            this->text.parse(trimmed(text));
        }
        else
        {
            for (auto& el : this->elements)
                el->parent = this;
        }
        this->fixParagraphs();
        this->buffer.clear();
    }

    void ListItem::fixParagraphs()
    {
        Type previousType = Type::Blank;
        std::vector<size_t> lineBreaks;
        for (auto it = this->elements.begin(); it != this->elements.end(); it++)
        {
            auto& el = *it;
            if (el->getType() == Type::Paragraph)
                el->options |= ElementOptions::Raw;

            if (previousType == el->getType() && previousType == Type::Paragraph)
                lineBreaks.push_back(it - this->elements.begin());

            previousType = el->getType();
        }

        for (auto it = lineBreaks.rbegin(); it != lineBreaks.rend(); it++)
        {
            auto lineBreak = *it;
            this->elements.addElement(std::make_shared<LineBreakElement>(), std::next(this->elements.begin(), lineBreak));
        }
    }

    std::string ListItem::getText() const
    {
        if (this->elements.empty())
            return this->text.getText();

        std::string str;
        for (const auto& element : this->elements)
        {
            if (element->getType() == Type::LineBreak)
                continue;

            str += element->getText() + "\n";
        }

        if (!str.empty())
            str.pop_back();
        return str;
    }

    std::string ListItem::getHtml() const
    {
        std::string html = "<li>";
        if (this->elements.empty())
            html += this->text.getHtml();
        else
        {
            for (const auto& element : this->elements)
            {
                html += element->getHtml();
            }
        }
        html += "</li>";
        return html;
    }

    std::string ListItem::dump(int indent) const
    {
        std::string result = Element::dump(indent) + "\n";
        result += this->elements.dump(indent);
        return result;
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
        ListMarker marker = getListLevel(line);
        int listLevel = marker.level;

        if (listLevel >= 0)
        {
            if (!this->elements.empty())
                this->elements.back()->finalize();

            auto item = std::make_shared<ListItem>();
            item->buffer += line;
            item->parent = this;
            
            this->elements.addElement(item);
            this->listType = marker.type;
            return ParseResult(ParseCode::RequestMore);
        }

        int indentation = getListIndentation(line);

        if (indentation >= 0)
        {
            if (this->elements.empty())
                return ParseResult(ParseCode::Invalid);

            auto lastItem = this->elements.back();
            auto lastItemLevel = this->elements.back()->getLevel();
            if (indentation < lastItemLevel)
                return ParseResult(ParseCode::Invalid);

            std::static_pointer_cast<ListItem>(lastItem)->buffer += "\n" + line;

            return ParseResult(ParseCode::RequestMore);
        }

        if (!this->elements.empty())
            this->elements.back()->finalize();

        return ParseResult(ParseCode::Invalid);
    }

    void ListElement::finalize()
    {
        if (!this->elements.empty())
            this->elements.back()->finalize();
    }

    std::string ListElement::getText() const
    {
        std::string str;
        unsigned int number = 1;

        switch (this->listType)
        {
        case ListType::Ordered:
            for (const auto& element : this->elements)
            {
                str += std::to_string(number++) + ". " + element->getText() + "\n";
            }
            break;
        
        case ListType::Unordered:
            for (const auto& element : this->elements)
            {
                str += "- " + element->getText() + "\n";
            }
            break;
        }
        if (!str.empty())
            str.pop_back();
        return str;
    }

    std::string ListElement::getHtml() const
    {
        std::pair<std::string, std::string> tag;
        switch (this->listType)
        {
        case ListType::Ordered: tag = {"<ol>", "</ol>"}; break;
        case ListType::Unordered: tag = {"<ul>", "</ul>"}; break;
        default: tag = { "", "" }; assert(!"Invalid list type"); break;
        }

        std::string html = tag.first;
        for (const auto& element : this->elements)
        {
            html += element->getHtml();
        }
        html += tag.second;
        return html;
    }

    std::string ListElement::dump(int indent) const
    {
        std::string result = Element::dump(indent) + "\n";
        result += this->elements.dump(indent);
        return result;
    }

    size_t ListElement::findUnorderedMarker(const std::string& text)
    {
        std::vector<char> markers{'-', '*', '+'};
        for (char c : markers)
        {
            size_t pos = text.find_first_of(c);
            if (pos != std::string::npos)
            {
                if (pos >= text.size() - 1 || text[pos + 1] == c)
                    continue;

                return pos;
            }
        }
        return std::string::npos;
    }

    ListElement::ListMarker ListElement::getListLevel(const std::string& line)
    {
        ListMarker marker;

        auto pos = findUnorderedMarker(line);
        if (pos != std::string::npos)
            marker.type = ListType::Unordered;
        else
        {
            marker.type = ListType::Ordered;

            pos = line.find_first_of('.');
            if (pos == std::string::npos)
                return marker;

            for (auto it = line.begin() + pos; it != std::next(line.begin(), pos); it++)
            {
                if (!std::isdigit(*it))
                    return marker;
            }
        }

        bool tabs = line.front() == '\t';
        char search = tabs ? '\t' : ' ';
        unsigned int count = std::count(line.begin(), std::next(line.begin(), pos), search);

        marker.level = tabs ? count : count / 4;
        return marker;
    }

    int ListElement::getListIndentation(const std::string& line)
    {
        if (line.empty())
            return -1;

        bool tabs = line.front() == '\t';
        char search = tabs ? '\t' : ' ';
        int first = line.find_first_not_of(search);
        int count = std::count(line.begin(), std::next(line.begin(), first), search);

        return tabs ? count : count / 4;
    }

    std::string ListElement::getListText(const std::string& line)
    {
        auto pos = line.find_first_of('.');
        if (pos == std::string::npos)
        {
            pos = findUnorderedMarker(line);
            if (pos == std::string::npos)
                return "";
        }

        pos = line.find_first_not_of(' ', pos);
        return line.substr(pos + 1);
    }

    std::string ListElement::getListItemLineText(const std::string& line)
    {
        if (line.empty())
            return line;

        bool tabs = line.front() == '\t';
        char search = tabs ? '\t' : ' ';
        int posNonWhitespace = line.find_first_not_of(search);

        auto pos = line.find_first_of('.');
        if (pos == std::string::npos)
            pos = findUnorderedMarker(line);

        if (pos != std::string::npos)
        {
            std::string num = line.substr(posNonWhitespace, pos);
            if (isNumber(num.begin(), num.end()))
                return line.substr(pos + 1);
        }

        return line.substr(posNonWhitespace);
    }

    std::string ListElement::getListItemText(const std::string& line)
    {
        if (line.empty())
            return line;

        std::string result;

        std::stringstream str(line);
        for (std::string subLine; std::getline(str, subLine); )
        {
            result += getListItemLineText(subLine) + "\n";
        }

        if (!result.empty())
            result.pop_back();

        return result;
    }
}