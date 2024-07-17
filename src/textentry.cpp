#include "textentry.h"

#include <queue>
#include <unordered_map>
#include <regex>

namespace Markdown
{
	using StyleContainer = std::vector<std::shared_ptr<MarkdownStyle>>;

	// Generic style

	MarkdownStyle::Result GenericStyle::findIn(const std::string& str, size_t offset, SearchMode mode, size_t level) const
	{
		size_t beginSize = this->markdownOpening.size();
		size_t endSize = this->markdownClosing.size();

		size_t begin = str.find(this->markdownOpening, offset);
		size_t end = str.find(this->markdownClosing, begin + beginSize);

		size_t textBegin = begin + beginSize;
		size_t textEnd = end - endSize;

		if (begin != std::string::npos && end != std::string::npos)
		{
			return {
				begin,
				end - begin + endSize,
				std::make_unique<TextEntry::Span>(str.substr(textBegin, textEnd - textBegin + endSize), std::make_shared<GenericStyle>(*this), level)
			};
		}

		return {};
	}

	// Generic span

	TextEntry::Span::Span(const std::string& text, std::shared_ptr<MarkdownStyle> style, size_t level)
		: text(text)
		, style(style)
		, level(level)
	{ }

	std::string TextEntry::Span::getOpeningTag()
	{
		return this->style->style.openingTag;
	}

	std::string TextEntry::Span::getClosingTag()
	{
		return this->style->style.closingTag;
	}

	std::string TextEntry::Span::getText() const
	{
		return this->text;
	}

	std::string TextEntry::Span::getHtml() const
	{
		return this->style->style.openingTag + this->text + this->style->style.closingTag;
	}

	// Link style

	MarkdownStyle::Result LinkStyle::findIn(const std::string& str, size_t offset, SearchMode mode, size_t level) const
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
				std::make_unique<LinkSpan>(linktext, linkurl, std::make_shared<LinkStyle>(*this), level)
			};
		}

		return { };
	}

	// Link span

	LinkStyle::LinkSpan::LinkSpan(const std::string& text, const std::string& url, std::shared_ptr<MarkdownStyle> style, size_t level)
		: Span(text, style, level)
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

	// Util

	MarkdownStyle::Result findFirst(
		const std::string& source,
		size_t pos,
		const StyleContainer& stylemap,
		size_t level,
		MarkdownStyle::SearchMode mode = MarkdownStyle::SearchMode::Opening,
		const std::vector<std::string>& autoescape = {}
	)
	{
		MarkdownStyle::Result result;

		for (const auto& style : stylemap)
		{
			if (std::find(autoescape.begin(), autoescape.end(), style->markdownOpening) != autoescape.end())
				continue; // Autoescape

			MarkdownStyle::Result candidate = style->findIn(source, pos, mode, level);
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

	// Text entry

	std::vector<std::unique_ptr<TextEntry::Span>> findStyle(
		const std::string& source,
		size_t level = 0,
		const std::vector<std::string>& autoescape = {}
	)
	{
		// TODO - gotta split this function

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

		std::vector<std::unique_ptr<TextEntry::Span>> spans;

		size_t pos = 0;

		while (pos != std::string::npos)
		{
			MarkdownStyle::Result style = findFirst(source, pos, stylemap, level, MarkdownStyle::SearchMode::Opening, autoescape);
			if (!style)
				break;

			// If some characters were skipped fill in the blank with empty style span
			if (style.position != pos)
				spans.emplace_back(std::make_unique<TextEntry::Span>(source.substr(pos, style.position - pos), nullptr));

			pos = style.position + style.length;

			spans.push_back(std::move(style.span));
		}

		// Fill in the rest of the source
		if (pos != source.size())
			spans.emplace_back(std::make_unique<TextEntry::Span>(source.substr(pos), nullptr));

		// If no spans were found add an empty style span
		if (spans.empty())
			spans.emplace_back(std::make_unique<TextEntry::Span>(source, nullptr));

		return spans;
	}

	TextEntry::TextEntry(const std::string& content, const std::shared_ptr<MarkdownStyle> defaultStyle)
	{
		if (content.empty())
			return;

		std::vector<std::unique_ptr<TextEntry::Span>> spans = findStyle(content);

		if (defaultStyle)
		{
			this->spans.emplace_back(std::make_unique<Span>("", defaultStyle));

			for (auto& span : spans)
			{
				this->spans.back()->children.emplace_back(std::move(span));
				//this->spans.emplace_back(content.substr(span.from, span.to - span.from), span.style, span.level);
			}
		}
		else
		{
			for (auto& span : spans)
			{
				this->spans.emplace_back(std::move(span));
				//this->spans.emplace_back(content.substr(span.from, span.to - span.from), span.style, span.level);
			}
		}
	}

	std::string TextEntry::getText() const
	{
		std::string text;
		for (const auto& span : this->spans)
		{
			text += span->text;
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
			if (!skipDefaultTags && (tags.empty() || span->level > tags.back().first))
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

			html += span->text;
		}

		if (!skipDefaultTags && !tags.empty())
		{
			Style closingTag = tags.back().second;
			html += closingTag.closingTag;
		}

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