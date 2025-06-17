#include "cppmarkdown/paragraphelement.h"
#include "cppmarkdown/linebreakelement.h"

#include <vector>

namespace Markdown
{
	std::shared_ptr<MarkdownStyle> ParagraphElement::getParagraphStyle() const
	{
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

		if (text.empty())
			return ParseResult(ParseCode::Invalid);

		this->text = TextEntry(text, getParagraphStyle());

		return ParseResult(ParseCode::ElementComplete);
	}

	FinalizeAction ParagraphElement::documentFinalize(std::shared_ptr<Element> previous)
	{
		if (previous && previous->getType() == Type::Paragraph)
		{
			auto previousParagraph = std::static_pointer_cast<ParagraphElement>(previous);
			
			if (!text.empty() && !previousParagraph->text.empty())
			{
				this->text = TextEntry(previousParagraph->getMarkdown() + " " + this->getMarkdown(), getParagraphStyle());
				return FinalizeAction::ErasePrevious | FinalizeAction::Continue;
			}
		}

		if (previous && previous->getType() == Type::LineBreak && std::static_pointer_cast<LineBreakElement>(previous)->skippable)
			return FinalizeAction::ErasePrevious;

		return FinalizeAction::None;
	}

	std::string ParagraphElement::getText() const
	{
		return this->text.getText();
	}

	std::string ParagraphElement::getHtml() const
	{
		return this->text.getHtml();
	}

	std::string ParagraphElement::getInnerHtml() const
	{
		return this->text.getInnerHtml();
	}

	std::string ParagraphElement::getMarkdown() const
	{
		return this->text.getMarkdown();
	}
}