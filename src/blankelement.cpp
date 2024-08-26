#include "blankelement.h"

#include <sstream>
#include <cassert>

namespace Markdown
{
    BlankElement::BlankElement(const std::string& text)
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

    Type BlankElement::getType() const
    {
        return Type::Blank;
    }

    ParseResult BlankElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        if (line.empty())
            return ParseResult(ParseCode::Invalid);

        this->elements.parse(line);
        return ParseResult(ParseCode::ElementComplete);
    }

    void BlankElement::finalize()
    {
        if (!this->elements.empty())
            this->elements.back()->finalize();
    }

    std::string BlankElement::getText() const
    {
        std::string str;
        for (const auto& el : this->elements)
        {
            str += el->getText() + " ";
        }
        if (!str.empty())
            str.pop_back();
        return str;
    }

    std::string BlankElement::getHtml() const
    {
        std::string html;
        for (const auto& element : this->elements)
        {
            html += element->getHtml();
        }
        return html;
    }
}