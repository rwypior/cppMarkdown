#include "cppmarkdown.h"

#include <queue>

namespace Markdown
{
	using StyleTagPair = std::pair<std::string, std::string>;
	using StyleTag = std::pair<StyleTagPair, TextEntry::Style>;
	using StyleMap = std::vector<StyleTag>;

	struct StyleSpan
	{
		size_t from;
		size_t to;
		TextEntry::Style style;

		StyleSpan(size_t from, size_t to, TextEntry::Style style)
			: from(from)
			, to(to)
			, style(style)
		{ }
	};

	std::pair<StyleTag, size_t> findFirst(const std::string& source, size_t pos, const StyleMap& stylemap, bool useSecond = false)
	{
		std::pair<StyleTag, size_t> result{ {}, std::string::npos };

		for (const StyleTag& t : stylemap)
		{
			std::string tofind = useSecond ? t.first.second : t.first.first;
			size_t idx = source.find(tofind, pos);
			if (idx != std::string::npos && idx < result.second)
			{
				result.first = t;
				result.second = idx;

				if (idx == 0)
					break;
			}
		}

		return result;
	}

	std::vector<StyleSpan> findStyle(const std::string& source)
	{
		StyleMap stylemap{
			{ {"***", "***"}, TextEntry::Style::BoldItalic },
			{ {"___", "___"}, TextEntry::Style::BoldItalic },
			{ {"__*", "*__"}, TextEntry::Style::BoldItalic },
			{ {"**_", "_**"}, TextEntry::Style::BoldItalic },
			{ {"**", "**"}, TextEntry::Style::Bold },
			{ {"__", "__"}, TextEntry::Style::Bold },
			{ {"*", "*"}, TextEntry::Style::Italic },
			{ {"_", "_"}, TextEntry::Style::Italic }
		};

		std::vector<StyleSpan> styles;

		size_t pos = 0;

		while (pos != std::string::npos)
		{
			// Find first style tag
			std::pair<StyleTag, size_t> begin = findFirst(source, pos, stylemap);
			if (begin.second == std::string::npos)
				break;

			// Add the style tag to the result vector
			StyleTag tag = begin.first;
			std::string ending = tag.first.second;
			pos = begin.second + tag.first.first.size();

			styles.emplace_back(pos, std::string::npos, tag.second);

			// Find matching ending tag
			size_t idx = source.find(ending, pos);

			// Check if found ending tag isn't part of longer tag
			size_t recheckIdx = idx;
			while (recheckIdx != std::string::npos)
			{
				std::pair<StyleTag, size_t> foundEnding = findFirst(source, recheckIdx, stylemap, true);
				std::string endingStr = foundEnding.first.first.second;
				if (endingStr == ending)
				{
					idx = foundEnding.second;
					break;
				}
				else
					recheckIdx = foundEnding.second + endingStr.size();
			}

			if (idx != std::string::npos)
			{
				styles.back().to = idx - ending.size();
				pos = idx + ending.size();
			}
		}

		// Find substyles and merge
		for (StyleSpan& span : styles)
		{
			std::vector<StyleSpan> subStyles = findStyle(source.substr(span.from, span.to - span.from));
		}

		// TODO - merge found substyles

		return styles;
	}

	TextEntry::TextEntry(const std::string& content)
	{
		if (content.empty())
			return;

		std::vector<StyleSpan> styles = findStyle(content);
	}

	std::string TextEntry::getText() const
	{
		std::string text;
		for (const Span& span : this->spans)
		{
			text += span.text + " ";
		}

		if (!text.empty())
			text.pop_back();

		return text;
	}

	std::string TextEntry::getHtml() const
	{
		std::string html;
		for (const Span& span : this->spans)
		{
			std::string spanHtml = span.text;
			switch (span.style)
			{
			case Style::Bold: spanHtml = "<b>" + spanHtml + "</b>"; break;
			case Style::Italic: spanHtml = "<i>" + spanHtml + "</i>"; break;
			case Style::BoldItalic: spanHtml = "<b><i>" + spanHtml + "</i></b>"; break;
			}
			html += spanHtml + " ";
		}

		if (!html.empty())
			html.pop_back();

		return html;
	}

	std::string TextEntry::getRawText() const
	{
		std::string raw;
		for (const Span& span : this->spans)
		{
			std::string spanRaw = span.text;
			switch (span.style)
			{
			case Style::Bold: spanRaw = "**" + spanRaw + "**"; break;
			case Style::Italic: spanRaw = "*" + spanRaw + "*"; break;
			case Style::BoldItalic: spanRaw = "***" + spanRaw + "***"; break;
			}
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