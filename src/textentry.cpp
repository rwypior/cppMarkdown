#include "textentry.h"

#include <queue>
#include <unordered_map>
#include <regex>

namespace Markdown
{
	// Util

	MarkdownStyle::Result findFirst(
		const std::string& source,
		size_t pos,
		const StyleContainer& stylemap,
		const std::vector<std::string>& autoescape = {}
	)
	{
		MarkdownStyle::Result result;

		for (const auto& style : stylemap)
		{
			if (std::find(autoescape.begin(), autoescape.end(), style->markdownOpening) != autoescape.end())
				continue; // Autoescape

			MarkdownStyle::Result candidate = style->findIn(source, pos, stylemap);
			size_t idx = candidate.position;
			if (idx != std::string::npos && idx < result.position)
			{
				result = std::move(candidate);

				if (idx == 0)
					break;
			}
		}

		return result;
	}

	// Generic style

	MarkdownStyle::Result GenericStyle::findIn(const std::string& str, size_t offset, const StyleContainer& stylemap) const
	{
		size_t beginSize = this->markdownOpening.size();
		size_t endSize = this->markdownClosing.size();

		size_t begin = str.find(this->markdownOpening, offset);
		size_t end = str.find(this->markdownClosing, begin + beginSize);

		if (end != std::string::npos)
		{
			// Recheck if found tag is not part of a larger tag and try to find better alternative
			bool good = false;
			size_t recheck = end;
			while (recheck != std::string::npos && !good )
			{
				for (const auto& style : stylemap)
				{
					size_t foundIdx = str.find(style->markdownOpening, recheck);
					bool found = foundIdx == recheck;
					good = style->markdownOpening == this->markdownOpening;
					if (found && !good)
					{
						recheck = str.find(this->markdownClosing, recheck + style->markdownOpening.size());
						//recheck += style->markdownOpening.size();
						break;
					}
					else if (found)
					{
						end = recheck;
						break;
					}
				}
			}
		}

		size_t textBegin = begin + beginSize;
		size_t textEnd = end - endSize;

		if (begin != std::string::npos && end != std::string::npos)
		{
			return {
				begin,
				end - begin + endSize,
				std::make_unique<Span>(str.substr(textBegin, textEnd - textBegin + endSize), std::make_shared<GenericStyle>(*this))
			};
		}

		return {};
	}

	// Span container

	void SpanContainer::parse(const std::string& markdown, const std::shared_ptr<MarkdownStyle> defaultStyle, SpanSearchFlags searchFlags)
	{
		if (markdown.empty())
			return;

		std::vector<std::unique_ptr<Span>> foundSpans = TextEntry::findStyle(markdown, 0, {}, searchFlags);

		auto& spans = this->getSpans();
		if (defaultStyle)
		{
			spans.emplace_back(std::make_unique<Span>("", defaultStyle));

			for (auto& span : foundSpans)
			{
				spans.back()->children.emplace_back(std::move(span));
				//this->spans.emplace_back(content.substr(span.from, span.to - span.from), span.style, span.level);
			}
		}
		else
		{
			for (auto& span : foundSpans)
			{
				spans.emplace_back(std::move(span));
				//this->spans.emplace_back(content.substr(span.from, span.to - span.from), span.style, span.level);
			}
		}

		for (auto& span : spans)
		{
			span->parse(span->getText(), nullptr, SpanSearchFlags::Normal);
		}
	}

	// Generic span

	Span::Span(const std::string& text, std::shared_ptr<MarkdownStyle> style)
		: text(text)
		, style(style)
	{ }

	SpanContainer::Container& Span::getSpans()
	{
		return this->children;
	}

	const SpanContainer::Container& Span::getSpans() const
	{
		return this->children;
	}

	std::string Span::getOpeningTag()
	{
		return this->style->style.openingTag;
	}

	std::string Span::getClosingTag()
	{
		return this->style->style.closingTag;
	}

	std::string Span::getText() const
	{
		if (this->hasSpans())
		{
			std::string result;
			for (const auto& span : this->getSpans())
			{
				result += span->getText();
			}
			return result;
		}
		else
			return this->text;
	}

	std::string Span::getHtml() const
	{
		if (this->style)
		{
			std::string text;
			if (this->hasSpans())
			{
				for (const auto& span : this->getSpans())
				{
					text += span->getHtml();
				}
			}
			else
				text = this->text;

			return this->style->style.openingTag + text + this->style->style.closingTag;
		}
		else
			return this->getText();
	}

	// Link style

	MarkdownStyle::Result LinkStyle::findIn(const std::string& str, size_t offset, const StyleContainer& /*stylemap*/) const
	{
		std::regex regex(R"r(.*(\[(.+?)\]\((.*?)\)).*)r");
		std::smatch matches;
		if (std::regex_match(std::next(str.begin(), offset), str.end(), matches, regex))
		{
			size_t pos = matches.position(1);
			std::string full = std::next(matches.begin(), 1)->str();
			std::string linktext = std::next(matches.begin(), 2)->str();
			std::string linkurl = std::next(matches.begin(), 3)->str();

			return {
				pos,
				full.size(),
				std::make_unique<LinkSpan>(linktext, linkurl, std::make_shared<LinkStyle>(*this))
			};
		}

		return { };
	}

	// Link span

	LinkStyle::LinkSpan::LinkSpan(const std::string& text, const std::string& url, std::shared_ptr<MarkdownStyle> style)
		: Span(text, style)
		, url(url)
	{ }

	std::string LinkStyle::LinkSpan::getText() const
	{
		return this->text;
	}

	std::string LinkStyle::LinkSpan::getHtml() const
	{
		return "<a href=\"" + this->url + "\">" + this->text + "</a>";
	}

	// Text entry

	std::vector<std::unique_ptr<Span>> TextEntry::findStyle(
		const std::string& source,
		size_t level,
		const std::vector<std::string>& autoescape,
		SpanSearchFlags flags
	)
	{
		StyleContainer stylemap{
			std::make_shared<GenericStyle>("***", "***", Style("<b><i>", "</i></b>")),
			std::make_shared<GenericStyle>("___", "___", Style("<b><i>", "</i></b>")),
			std::make_shared<GenericStyle>("__*", "*__", Style("<b><i>", "</i></b>")),
			std::make_shared<GenericStyle>("**_", "_**", Style("<b><i>", "</i></b>")),
			std::make_shared<GenericStyle>("**", "**", Style("<b>", "</b>")),
			std::make_shared<GenericStyle>("__", "__", Style("<b>", "</b>")),
			std::make_shared<GenericStyle>("``", "``", Style("<code>", "</code>"), false, std::vector<std::string>({"`"})),
			std::make_shared<GenericStyle>("*", "*", Style("<i>", "</i>")),
			std::make_shared<GenericStyle>("_", "_", Style("<i>", "</i>")),
			std::make_shared<GenericStyle>("`", "`", Style("<code>", "</code>"), false),
			std::make_shared<LinkStyle>(),
		};

		std::vector<std::unique_ptr<Span>> spans;

		size_t pos = 0;

		while (pos != std::string::npos)
		{
			MarkdownStyle::Result style = findFirst(source, pos, stylemap, autoescape);
			if (!style)
				break;

			std::string styleEndingStr = style.span->style->markdownClosing;
			size_t idx = style.position;

			// If some characters were skipped fill in the blank with empty style span
			if (idx != pos)
				spans.emplace_back(std::make_unique<Span>(source.substr(pos, idx - pos), nullptr));

			pos = idx + style.length;

			spans.push_back(std::move(style.span));
		}

		if (spans.empty())
		{
			if (flags & SpanSearchFlags::AddEmpty)
			{
				// If no spans were found add an empty style span
				spans.emplace_back(std::make_unique<Span>(source, nullptr));
			}
		}
		else if (pos != source.size())
			// Fill in the rest of the source
			spans.emplace_back(std::make_unique<Span>(source.substr(pos), nullptr));

		

		//// Fill in the rest of the source
		//if (pos != source.size())
		//	spans.emplace_back(std::make_unique<Span>(source.substr(pos), nullptr));

		//// If no spans were found add an empty style span
		//if (spans.empty())
		//	spans.emplace_back(std::make_unique<Span>(source, nullptr));

		return spans;
	}

	TextEntry::TextEntry(const std::string& content, const std::shared_ptr<MarkdownStyle> defaultStyle)
	{
		this->parse(content);
	}

	SpanContainer::Container& TextEntry::getSpans()
	{
		return this->spans;
	}

	const SpanContainer::Container& TextEntry::getSpans() const
	{
		return this->spans;
	}

	std::string TextEntry::getText() const
	{
		std::string text;
		for (const auto& span : this->spans)
		{
			text += span->getText();
		}

		return text;
	}

	// TODO:
	// span.text contains the markdown opening
	// it has to contain only the text

	std::string TextEntry::getHtml(HtmlOptions flags) const
	{
		std::deque<std::pair<size_t, Style>> tags;

		bool skipDefaultTags = flags & HtmlOptions::SkipDefaultTags;

		std::string html;
		for (const auto& span : this->spans)
		{
			html += span->getHtml();

			/*if (!skipDefaultTags && (tags.empty() || span->level > tags.back().first))
			{
				tags.push_back({ span->level, span->style->style });
				html += span->style->style.openingTag;
			}
			else if (!tags.empty())
			{
				Style closingTag = tags.back().second;
				tags.pop_back();
				html += closingTag.closingTag;
			}

			html += span->text;*/
		}

		/*if (!skipDefaultTags && !tags.empty())
		{
			Style closingTag = tags.back().second;
			html += closingTag.closingTag;
		}*/

		return html;
	}

	std::string TextEntry::getRawText() const
	{
		std::string raw;
		for (const auto& span : this->spans)
		{
			std::string spanRaw = span->style->markdownOpening + span->text + span->style->markdownClosing;
			raw += spanRaw + " ";
		}

		if (!raw.empty())
			raw.pop_back();

		return raw;
	}

	bool TextEntry::empty() const
	{
		return this->spans.empty();
	}
}