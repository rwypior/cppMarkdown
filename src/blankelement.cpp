#include "blankelement.h"

#include <sstream>
#include <cassert>

namespace Markdown
{
    BlankElement::BlankElement(const std::string& text)
        : text(text)
    {
    }

    Type BlankElement::getType() const
    {
        return Type::Blank;
    }

    ParseResult BlankElement::parse(const std::string&, std::shared_ptr<Element>)
    {
        return ParseResult(ParseCode::ElementComplete);
    }

    std::string BlankElement::getText() const
    {
        return this->text;
    }

    std::string BlankElement::getHtml() const
    {
        return this->text;
    }

    // Util

    std::shared_ptr<BlankElement> toBlankElement(const Element& element)
    {
        return std::make_shared<BlankElement>(element.getText());
    }
}