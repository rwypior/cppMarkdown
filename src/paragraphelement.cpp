#include "cppmarkdown.h"

namespace Markdown
{
	Type ParagraphElement::getType() const
	{
		return Type::Paragraph;
	}

	ParseResult ParagraphElement::parse(const std::string& line, std::shared_ptr<Element> previous)
	{
		return ParseResult();
	}
}