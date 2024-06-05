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
		size_t tagFrom;
		size_t tagTo;
		TextEntry::Style style;

		StyleSpan(size_t from, size_t to, size_t tagFrom, size_t tagTo, TextEntry::Style style)
			: from(from)
			, to(to)
			, tagFrom(tagFrom)
			, tagTo(tagTo)
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

	std::vector<StyleSpan> findStyle(const std::string& source, TextEntry::Style defaultStyle = TextEntry::Style::Normal)
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

			styles.emplace_back(pos, std::string::npos, begin.second, std::string::npos, tag.second);

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
				styles.back().to = idx - 1;
				styles.back().tagTo = idx + ending.size() - 1;
				pos = idx + ending.size();
			}
		}

		// Find substyles
		std::deque<StyleSpan> mergeQueue;
		for (StyleSpan& span : styles)
		{
			std::vector<StyleSpan> subStyles = findStyle(source.substr(span.from, span.to - span.from), span.style);
			for (StyleSpan& sub : subStyles)
			{
				//mergeQueue.push_back( StyleSpan(span.from + sub.from, span.from + sub.to, span.tagFrom + sub.tagFrom, span.from + sub.tagTo, sub.style));
				mergeQueue.push_back( StyleSpan(sub.tagFrom + span.from, sub.tagFrom + span.to, sub.tagFrom + span.tagFrom, sub.tagFrom + span.tagTo, sub.style));
			}
		}

		// Merge substyles
		std::vector<StyleSpan> stylesMerged;
		while (!mergeQueue.empty())
		{
			auto entry = mergeQueue.front();
			mergeQueue.pop_front();
			stylesMerged.push_back(entry);
			//for (auto it = styles.begin(); it != styles.end(); it++)
			//{
			//	StyleSpan& style = *it;
			//	if (entry.from > style.from)
			//	{
			//		styles.insert(it, entry);
			//		break;
			//	}
			//}
		}

		if (!stylesMerged.empty())
			styles = stylesMerged;

		// Fill in default styles
		std::deque<std::pair<size_t, StyleSpan>> fillQueue;

		pos = 0;
		StyleSpan* span = nullptr;
		for (auto it = styles.begin(); it != styles.end(); it++)
		{
			span = &*it;
			if (span->tagFrom > pos)
				fillQueue.push_back({ std::distance(styles.begin(), it), StyleSpan(pos, span->tagFrom, pos, span->tagFrom, defaultStyle) });
			pos = span->tagTo;
		}

		if (span && span->tagTo < source.length())
			fillQueue.push_back({ styles.size(), StyleSpan(span->tagTo + 1, source.length(), span->tagTo + 1, source.length(), defaultStyle)});

		while (!fillQueue.empty())
		{
			auto entry = fillQueue.back();
			fillQueue.pop_back();
			auto it = styles.begin() + entry.first;
			styles.insert(it, entry.second);
		}

		if (styles.empty())
			styles.emplace_back(0, source.size(), 0, source.size(), defaultStyle);

		return styles;
	}

	TextEntry::TextEntry(const std::string& content)
	{
		if (content.empty())
			return;

		std::vector<StyleSpan> styles = findStyle(content);
		for (StyleSpan& span : styles)
		{
			this->spans.emplace_back(content.substr(span.from, span.to - span.from), span.style);
		}
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