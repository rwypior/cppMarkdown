#include "textentry.h"

#include <queue>
#include <unordered_map>

namespace Markdown
{
	using StyleContainer = std::vector<MarkdownStyle>;
	using StylePositionPair = std::pair<const MarkdownStyle*, size_t>;

	struct StyleSpan
	{
		size_t from;
		size_t to;
		size_t tagFrom;
		size_t tagTo;
		MarkdownStyle style;
		size_t level;

		StyleSpan(size_t from, size_t to, size_t tagFrom, size_t tagTo, const MarkdownStyle& style, size_t level)
			: from(from)
			, to(to)
			, tagFrom(tagFrom)
			, tagTo(tagTo)
			, style(style)
			, level(level)
		{ }
	};

	StylePositionPair findFirst(const std::string& source, size_t pos, const StyleContainer& stylemap, bool findClosing = false)
	{
		StylePositionPair result{ nullptr, std::string::npos };

		for (const MarkdownStyle& t : stylemap)
		{
			std::string tofind = findClosing ? t.markdownClosing : t.markdownOpening;
			size_t idx = source.find(tofind, pos);
			if (idx != std::string::npos && idx < result.second)
			{
				result.first = &t;
				result.second = idx;

				if (idx == 0)
					break;
			}
		}

		return result;
	}

	std::vector<StyleSpan> findStyle(const std::string& source, MarkdownStyle defaultStyle = MarkdownStyle(), size_t level = 0)
	{
		// TODO - gotta split this function

		StyleContainer stylemap{
			{ "***", "***", { "<b><i>", "</i></b>" } },
			{ "___", "___", { "<b><i>", "</i></b>" } },
			{ "__*", "*__", { "<b><i>", "</i></b>" } },
			{ "**_", "_**", { "<b><i>", "</i></b>" } },
			{ "**", "**", { "<b>", "</b>" } },
			{ "__", "__", { "<b>", "</b>" } },
			{ "*", "*", { "<i>", "</i>" } },
			{ "_", "_", { "<i>", "</i>" } },
		};

		std::vector<StyleSpan> styles;

		size_t pos = 0;

		while (pos != std::string::npos)
		{
			// Find first style tag
			StylePositionPair begin = findFirst(source, pos, stylemap);
			if (begin.second == std::string::npos)
				break;

			// Add the style tag to the result vector
			const MarkdownStyle *tag = begin.first;
			std::string ending = tag->markdownClosing;
			pos = begin.second + tag->markdownOpening.size();

			styles.emplace_back(pos, std::string::npos, begin.second, std::string::npos, *tag, level);

			// Find matching ending tag
			size_t idx = source.find(ending, pos);

			// Check if found ending tag isn't part of longer tag
			size_t recheckIdx = idx;
			while (recheckIdx != std::string::npos)
			{
				StylePositionPair foundEnding = findFirst(source, recheckIdx, stylemap, true);
				std::string endingStr = foundEnding.first->markdownClosing;
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
				styles.back().to = idx;
				styles.back().tagTo = idx + ending.size();
				pos = idx + ending.size();
			}
		}

		// Find substyles
		std::deque<StyleSpan> mergeQueue;
		for (StyleSpan& span : styles)
		{
			std::vector<StyleSpan> subStyles = findStyle(source.substr(span.from, span.to - span.from), span.style, level + 1);
			for (StyleSpan& sub : subStyles)
			{
				size_t tagSize = span.tagTo - span.to;
				mergeQueue.push_back( StyleSpan(
					span.from + sub.from, span.from + sub.to, 
					span.tagFrom + sub.tagFrom, span.from + sub.tagTo + tagSize,
					sub.style, sub.level));
			}
		}

		// Merge substyles
		std::vector<StyleSpan> stylesMerged;
		while (!mergeQueue.empty())
		{
			auto entry = mergeQueue.front();
			mergeQueue.pop_front();
			stylesMerged.push_back(entry);
		}

		if (!stylesMerged.empty())
			styles = stylesMerged;

		// Fill in default styles
		std::deque<std::pair<size_t, StyleSpan>> fillQueue;

		size_t lastpos = 0;
		StyleSpan* span = nullptr;
		for (auto it = styles.begin(); it != styles.end(); it++)
		{
			span = &*it;
			if (span->tagFrom > lastpos)
				fillQueue.push_back({ std::distance(styles.begin(), it), StyleSpan(lastpos, span->tagFrom, lastpos, span->tagFrom, defaultStyle, level) });
			lastpos = span->tagTo;
		}

		if (span && span->tagTo <= source.length())
			fillQueue.push_back({ styles.size(), StyleSpan(span->tagTo, source.length(), span->tagTo, source.length(), defaultStyle, level)});

		while (!fillQueue.empty())
		{
			auto entry = fillQueue.back();
			fillQueue.pop_back();
			auto it = styles.begin() + entry.first;
			styles.insert(it, entry.second);
		}

		if (styles.empty())
			styles.emplace_back(0, source.size(), 0, source.size(), defaultStyle, level);

		return styles;
	}

	TextEntry::TextEntry(const std::string& content, MarkdownStyle defaultStyle)
	{
		if (content.empty())
			return;

		std::vector<StyleSpan> styles = findStyle(content, defaultStyle);
		for (StyleSpan& span : styles)
		{
			this->spans.emplace_back(content.substr(span.from, span.to - span.from), span.style, span.level);
		}
	}

	std::string TextEntry::getText() const
	{
		std::string text;
		for (const Span& span : this->spans)
		{
			text += span.text;
		}

		return text;
	}

	std::string TextEntry::getHtml() const
	{
		std::deque<std::pair<size_t, Style>> tags;
				
		std::string html;
		for (const Span& span : this->spans)
		{
			if (tags.empty() || span.level > tags.back().first)
			{
				tags.push_back({ span.level, span.style.style });
				html += span.style.style.openingTag;
			}
			else
			{
				Style closingTag = tags.back().second;
				tags.pop_back();
				html += closingTag.closingTag;
			}

			html += span.text;
		}

		if (!tags.empty())
		{
			Style closingTag = tags.back().second;
			html += closingTag.closingTag;
		}

		return html;
	}

	std::string TextEntry::getRawText() const
	{
		std::string raw;
		for (const Span& span : this->spans)
		{
			std::string spanRaw = span.style.markdownOpening + span.text + span.style.markdownClosing;
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