#include "cppmarkdown/listelement.h"
#include "cppmarkdown/linebreakelement.h"
#include "cppmarkdown/blankelement.h"

#include <sstream>
#include <cassert>
#include <algorithm>
#include <functional>

namespace Markdown
{
    // List item

    ListItem::ListItem(const std::string& text)
    {
        if (!text.empty())
            this->parse(ListElement::getListItemText(text), nullptr);
    }

    ListItem::ListItem(ListElement* parent)
        : parent(parent)
    {
    }

    int ListItem::getListLevel() const
    {
        return this->level;
    }

    bool ListItem::sublistFirst() const
    {
        return this->elements.empty() ? false : this->elements.front()->getType() == Type::List;
    }

    std::shared_ptr<Element> ListItem::getLastItem() const
    {
        if (this->elements.empty())
            return nullptr;

        return this->elements.back();
    }

    Type ListItem::getType() const
    {
        return Type::ListItem;
    }

    ParseResult ListItem::parse(const std::string& line, std::shared_ptr<Element>)
    {
        // We don't care about marker's validity here - ListElement checks it, only get the level
        ListElement::ListMarker marker = ListElement::getListLevel(line, true);
        this->level = marker.level;

        ElementContainer::parse(ListElement::getListItemText(line));
        this->buffer += line + "\n";
        return ParseResult(ParseCode::RequestMore);
    }

    ParseResult ListItem::supply(const std::string& line, std::shared_ptr<Element> previous)
    {
        ListElement::ListMarker marker = ListElement::getListLevel(line);

        auto lastItem = this->getLastItem();
        ParseResult result;
        if (auto container = std::dynamic_pointer_cast<ElementContainer>(lastItem))
        {
            std::string listText = ListElement::getListTextWithMarker(line);
            result = container->parseLine(listText, previous, previous);
            if (result.element)
                result.element->finalize();
        }

        auto lastParentItem = this->parent->getLastItem();
        if (!lastItem || marker.level <= lastParentItem->getListLevel())
        {
            return ParseResult(ParseCode::ElementCompleteParseNext);
        }

        if (result.code == ParseCode::Invalid)
        {
            ElementContainer::parse(ListElement::getListItemText(line));
            this->replaceElements(Type::Paragraph, toBlankElement);
        }

        this->buffer += line + "\n";
        return ParseResult(ParseCode::RequestMore);
    }

    void ListItem::finalize()
    {
        std::string text = ListElement::getListItemText(this->buffer);

        for (auto& el : this->elements)
            el->parent = this;

        if (!this->elements.empty() && this->elements.front()->getType() == Type::Paragraph)
            this->replaceElement(this->elements.begin(), std::make_shared<BlankElement>(this->elements.front()->getText()));

        this->iterate(prependBlankElementsWithNewlines);
    }

    ElementContainer& ListItem::getContainer()
    {
        return *this;
    }

    TextEntry& ListItem::getText()
    {
        return this->text;
    }

    std::string ListItem::getText() const
    {
        std::string text = this->text.getText();
        if (!text.empty())
            text += "\n";

        for (const auto& element : this->elements)
        {
            if (element->getType() == Type::LineBreak)
                continue;

            text += element->getText() + "\n";
        }

        if (!text.empty())
            text.pop_back();

        return text;
    }

    std::string ListItem::getHtml() const
    {
        std::string html = "<li>";
        html += this->text.getHtml();
        for (const auto& element : this->elements)
        {
            html += element->getHtml();
        }
        html += "</li>";

        return html;
    }

    std::string ListItem::dump(int indent) const
    {
        std::string result = Element::dump(indent) + "\n";
        result += ElementContainer::dump(indent);
        return result;
    }

    // List element

    ListElement::ListElement(const std::string& text)
    {
        ElementContainer::parse(text);
    }

    ParseResult ListElement::parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active, Type)
    {
        return active ? this->supply(line, previous) : this->parse(line, previous);
    }

    Type ListElement::getType() const
    {
        return Type::List;
    }

    void ListElement::addElement(std::shared_ptr<Element>)
    { }

    void ListElement::addElement(std::shared_ptr<Element>, Container::const_iterator)
    { }

    ParseResult ListElement::parse(const std::string& line, std::shared_ptr<Element>)
    {
        ListMarker marker = getListLevel(line);

        if (!marker)
            return ParseResult(ParseCode::Invalid);

        auto item = std::make_shared<ListItem>(this);
        auto result = item->parse(line, nullptr);
        result.element = item;

        if (result.code != ParseCode::Invalid)
        {
            this->listType = marker.type;
            ElementContainer::addElement(item);
        }

        return result;
    }

    ParseResult ListElement::supply(const std::string& line, std::shared_ptr<Element> previous)
    {
        auto result = this->getLastItem()->supply(line, nullptr);

        if (result.code == ParseCode::ElementCompleteParseNext)
        {
            this->getLastItem()->finalize();
            result = this->parse(line, previous);
        }

        if (result.code == ParseCode::Invalid)
        {
            result.code = ParseCode::ElementCompleteParseNext;
        }
        else if (result.code != ParseCode::RequestMore)
        {
            ElementContainer::addElement(result.element);
            result.code = ParseCode::RequestMore;
        }
        else
        {
            result.element = this->getLastItem();
        }

        return result;
    }

    void ListElement::finalize()
    {
        for (auto item : this->elements)
        {
            item->finalize();
        }
    }

    std::shared_ptr<ListItem> ListElement::getLastItem() const
    {
        if (this->elements.empty())
            return nullptr;

        return std::static_pointer_cast<ListItem>(this->elements.back());
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
                std::string marker = std::to_string(number++) + ". ";
                str += marker + element->getText() + "\n";
            }
            break;
        
        case ListType::Unordered:
            for (const auto& element : this->elements)
            {
                std::string marker = std::dynamic_pointer_cast<ListItem>(element)->sublistFirst() ? "" : "- ";
                str += marker + element->getText() + "\n";
            }
            break;
        default:
            assert(!"Type musn't be unspecified");
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
        default: assert(!"Invalid list type"); break;
        }

        std::string html = tag.first;
        for (const auto& element : this->elements)
        {
            html += element->getHtml();
        }
        html += tag.second;
        return html;
    }

    std::string ListElement::getMarkdown() const
    {
        return this->getText();
    }

    std::string ListElement::dump(int indent) const
    {
        return ElementContainer::dump(indent);
    }

    size_t ListElement::countLeadingSpaces(const std::string& text)
    {
        return std::distance(text.begin(), std::find_if(text.begin(), text.end(), [](auto x) { return !std::isspace(x); }));
    }

    size_t ListElement::findUnorderedMarker(const std::string& text)
    {
        std::vector<char> markers{'-', '*', '+'};
        for (char c : markers)
        {
            size_t pos = text.find_first_of(c);
            if (pos != std::string::npos)
            {
                if (
                    pos >= text.size() - 1 || 
                    text[pos + 1] == c ||
                    pos > 0 && !std::all_of(text.begin(), text.begin() + pos, [](char c){ return std::isspace(c); })
                    )
                    continue;

                return pos;
            }
        }
        return std::string::npos;
    }

    std::tuple<size_t, size_t> ListElement::findOrderedMarker(const std::string& text)
    {
        size_t pos = text.find_first_of('.');
        if (pos == std::string::npos)
            return { std::string::npos, std::string::npos };

        size_t numlen = 0;
        bool foundSpace = false;
        for (auto it = std::make_reverse_iterator(text.begin() + pos); it != text.rend(); it++)
        {
            // Find any digits in direct neighbourhood of the dot
            // Must only have digits right next to dot and then only spaces
            // If found any digits after spaces were found - it's not list

            if (std::isdigit(*it) && !foundSpace)
                numlen++;
            else if (numlen == 0 || !std::isspace(*it))
                return { std::string::npos, std::string::npos };
            else if (std::isspace(*it))
                foundSpace = true;
            else
                return { std::string::npos, std::string::npos };
        }

        return { pos, numlen };
    }

    size_t ListElement::findMarker(const std::string& text)
    {
        auto poslen = findOrderedMarker(text);
        auto pos = std::get<0>(poslen);
        if (pos == std::string::npos)
            pos = findUnorderedMarker(text);
        return pos;
    }

    ListElement::ListMarker ListElement::getListLevel(const std::string& line, bool skipValidation)
    {
        ListMarker marker;

        if (line.empty())
            return marker;

        size_t count = 0;

        if (!skipValidation)
        {
            auto pos = findUnorderedMarker(line);
            if (pos != std::string::npos)
            {
                marker.type = ListType::Unordered;
                count = pos;
            }
            else
            {
                auto poslen = findOrderedMarker(line);
                pos = std::get<0>(poslen);
                count = std::get<1>(poslen);

                if (pos != std::string::npos)
                {
                    marker.type = ListType::Ordered;
                    count = pos - count;
                }
            }
        }
        
        if (skipValidation || count == std::string::npos)
            count = countLeadingSpaces(line);

        bool tabs = line.front() == '\t';
        char search = tabs ? '\t' : ' ';

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
        auto pos = findMarker(line);
        if (pos == std::string::npos)
            return "";
        return line.substr(line.find_first_not_of(' ', pos + 1));
    }

    std::string ListElement::getListTextWithMarker(const std::string& line)
    {
        auto pos = findMarker(line);
        if (pos == std::string::npos)
            return "";
        size_t space = line.find_last_of(" \t", pos);
        return line.substr(space == std::string::npos ? 0 : space + 1);
    }

    std::string ListElement::getListItemLineText(const std::string& line)
    {
        if (line.empty())
            return line;

        bool tabs = line.front() == '\t';
        char search = tabs ? '\t' : ' ';
        int posNonWhitespace = line.find_first_not_of(search);

        auto poslen = findOrderedMarker(line);
        auto pos = std::get<0>(poslen);
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
