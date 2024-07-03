#include "textentry.h"

#include <queue>
#include <unordered_map>
#include <regex>

namespace Markdown
{
	using StyleContainer = std::vector<std::shared_ptr<MarkdownStyle>>;
	//using StylePositionPair = std::pair<std::shared_ptr<MarkdownStyle>, size_t>;

	struct StyleSearchResult
	{
		std::shared_ptr<MarkdownStyle> style;
		SpanPositionPair spanPosition;
	};

	// Style

	SpanPositionPair MarkdownStyle::findIn(const std::string& str, size_t offset, SearchMode mode, size_t level) const
	{
		size_t pos = str.find(mode == SearchMode::Closing ? this->markdownClosing : this->markdownOpening, offset);
		StyleSpan span(offset, std::string::npos, pos, std::string::npos, *this, level);
		return { std::make_shared<StyleSpan>(span), pos };
	}

	// Link style

	SpanPositionPair LinkStyle::findIn(const std::string& str, size_t offset, SearchMode mode, size_t level) const
	{
		if (mode == SearchMode::Closing)
			return { nullptr, str.find(')', offset) };

		std::regex regex(R"r(.*(\[(.+?)\]\((.*?)\)).*)r");
		std::smatch matches;
		if (std::regex_match(std::next(str.begin(), offset), str.end(), matches, regex))
		{
			size_t pos = matches.position(1);
			std::string full = std::next(matches.begin(), 1)->str();
			std::string linktext = std::next(matches.begin(), 2)->str();
			std::string linkurl = std::next(matches.begin(), 3)->str();

			std::string linktag = "<a href=\"" + linkurl + "\">";
			Style linkStyle(linktag, "</a>");
			//styles.push_back(StyleSpan(pos + 1, pos + 1 + linktext.size(), pos, pos + full.size(), MarkdownStyle("", "", linkStyle), level));
			//styles.push_back(StyleSpan(pos + 1, pos + 1 + linktext.size(), pos, pos + full.size(), MarkdownStyle("", "", linkStyle), level));

			StyleSpan span(offset, std::string::npos, pos, std::string::npos, *this, level);

			return { std::make_shared<StyleSpan>(span), pos };
		}

		return { nullptr, std::string::npos };
	}

	// Util

	StyleSearchResult findFirst(const std::string& source, size_t pos, const StyleContainer& stylemap, size_t level, MarkdownStyle::SearchMode mode = MarkdownStyle::SearchMode::Opening)
	{
		StyleSearchResult result;

		for (const auto& t : stylemap)
		{
			SpanPositionPair res = t->findIn(source, pos, mode, level);
			size_t idx = res.position;
			if (idx != std::string::npos && idx < result.spanPosition.position)
			{
				result.style = t;
				result.spanPosition = res;

				if (idx == 0)
					break;
			}
		}

		return result;
	}

	// Text entry

	std::vector<StyleSpan> findStyle(const std::string& source, MarkdownStyle defaultStyle = MarkdownStyle(), size_t level = 0)
	{
		// TODO - gotta split this function

		StyleContainer stylemap{
			std::make_shared<MarkdownStyle>("***", "***", Style("<b><i>", "</i></b>")),
			std::make_shared<MarkdownStyle>("___", "___", Style("<b><i>", "</i></b>")),
			std::make_shared<MarkdownStyle>("__*", "*__", Style("<b><i>", "</i></b>")),
			std::make_shared<MarkdownStyle>("**_", "_**", Style("<b><i>", "</i></b>")),
			std::make_shared<MarkdownStyle>("**", "**", Style("<b>", "</b>")),
			std::make_shared<MarkdownStyle>("__", "__", Style("<b>", "</b>")),
			std::make_shared<MarkdownStyle>("``", "``", Style("<code>", "</code>"), false, std::vector<std::string>({"`"})),
			std::make_shared<MarkdownStyle>("*", "*", Style("<i>", "</i>")),
			std::make_shared<MarkdownStyle>("_", "_", Style("<i>", "</i>")),
			std::make_shared<MarkdownStyle>("`", "`", Style("<code>", "</code>"), false),
			std::make_shared<LinkStyle>(),
		};

		std::vector<StyleSpan> styles;

		size_t pos = 0;

		while (pos != std::string::npos)
		{
			// TODO extract regex searching to another function
			//std::regex regex(R"r(.*(\[(.+?)\]\((.*?)\)).*)r");
			//std::smatch matches;
			//if (std::regex_match(std::next(source.begin(), pos), source.end(), matches, regex))
			//{
			//	pos = matches.position(1);
			//	std::string full = std::next(matches.begin(), 1)->str();
			//	std::string linktext = std::next(matches.begin(), 2)->str();
			//	std::string linkurl = std::next(matches.begin(), 3)->str();

			//	std::string linktag = "<a href=\"" + linkurl + "\">";
			//	Style linkStyle(linktag, "</a>");
			//	//styles.push_back(StyleSpan(pos + 1, pos + 1 + linktext.size(), pos, pos + full.size(), MarkdownStyle("", "", linkStyle), level));
			//	styles.push_back(StyleSpan(pos + 1, pos + 1 + linktext.size(), pos, pos + full.size(), MarkdownStyle("", "", linkStyle), level));

			//	pos += full.size();
			//	if (pos >= source.size())
			//		pos = std::string::npos;
			//	continue;
			//}

			// Find first style tag
			StyleSearchResult begin = findFirst(source, pos, stylemap, level);
			if (begin.spanPosition.position == std::string::npos)
				break;

			// Add the style tag to the result vector
			const auto tag = begin.style;
			std::string ending = tag->markdownClosing;
			pos = begin.spanPosition.position + tag->markdownOpening.size();

			StyleSpan span = *begin.spanPosition.styleSpan;
			span.from = pos;
			
			styles.push_back(span);
			//styles.push_back(begin.first->createSpan(pos, std::string::npos, begin.second, std::string::npos, tag, level));
			//styles.emplace_back(pos, std::string::npos, begin.second, std::string::npos, *tag, level);

			// Find matching ending tag
			size_t idx = source.find(ending, pos);

			// Check if found ending tag isn't part of longer tag
			size_t recheckIdx = idx;
			while (recheckIdx != std::string::npos)
			{
				StyleSearchResult foundEnding = findFirst(source, recheckIdx, stylemap, level, MarkdownStyle::SearchMode::Closing);
				std::string endingStr = foundEnding.style->markdownClosing;
				if (endingStr == ending)
				{
					idx = foundEnding.spanPosition.position;
					break;
				}
				else
					recheckIdx = foundEnding.spanPosition.position + endingStr.size();
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
				mergeQueue.push_back(StyleSpan(
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

	std::string TextEntry::getHtml(HtmlOptions flags) const
	{
		std::deque<std::pair<size_t, Style>> tags;

		bool skipDefaultTags = flags & HtmlOptions::SkipDefaultTags;
				
		std::string html;
		for (const Span& span : this->spans)
		{
			if (!skipDefaultTags && (tags.empty() || span.level > tags.back().first))
			{
				tags.push_back({ span.level, span.style.style });
				html += span.style.style.openingTag;
			}
			else if (!tags.empty())
			{
				Style closingTag = tags.back().second;
				tags.pop_back();
				html += closingTag.closingTag;
			}

			html += span.text;
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