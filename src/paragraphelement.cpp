#include "paragraphelement.h"

#include <vector>

namespace Markdown
{
	MarkdownStyle ParagraphElement::getParagraphStyle() const
	{
		if (this->options & ElementOptions::Raw)
			return MarkdownStyle::makeHtml("", "");
		
		return MarkdownStyle::makeHtml("<p>", "</p>");
	}

	ParagraphElement::ParagraphElement(const std::string& content)
		: text(content, getParagraphStyle())
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

			this->text = TextEntry(text, getParagraphStyle());

			return ParseResult(ParseCode::ParseNextAcceptPrevious);
		}

		this->text = TextEntry(text, getParagraphStyle());

		return ParseResult(ParseCode::ParseNext);
	}

	std::string ParagraphElement::getText() const
	{
		return this->text.getText();
	}

	std::string ParagraphElement::getHtml() const
	{
		TextEntry::HtmlOptions options = TextEntry::HtmlOptions::Normal;

		if (this->options & ElementOptions::Raw)
			options |= TextEntry::HtmlOptions::SkipDefaultTags;

		return this->text.getHtml(options);
	}
}