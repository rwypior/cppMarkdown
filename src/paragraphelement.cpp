#include "cppmarkdown.h"

#include <vector>

namespace Markdown
{
	ParagraphElement::ParagraphElement(const std::string& content)
		: text(content)
	{
	}

	Type ParagraphElement::getType() const
	{
		return Type::Paragraph;
	}

	ParseResult ParagraphElement::parse(const std::string& line, std::shared_ptr<Element> previous)
	{
		std::string text = getMarkdownText(line);

		if (previous && previous->getType() == Type::Paragraph)
		{
			if (
				text.empty() &&
				std::static_pointer_cast<ParagraphElement>(previous)->text.empty()
			)
			{
				return ParseResult(ParseCode::Discard);
			}

			return ParseResult(
				ParseCode::RequestMoreAcceptPrevious,
				std::make_shared<ParagraphElement>(text)
			);
		}

		return ParseResult(
			ParseCode::RequestMore,
			std::make_shared<ParagraphElement>(text)
		);
		
		return ParseResult();
	}
}