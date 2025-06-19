#include "cppmarkdown/html.h"

#include <cassert>

namespace Markdown
{
	Tag::Tag(const std::string& name)
		: result("<" + name)
	{
	}

	Tag& Tag::add(const std::string& name)
	{
		this->result += "><" + name;
		return *this;
	}

	Tag& Tag::addattribute(const std::string& name, const std::string& value)
	{
		this->result += " " + name + "=\"" + value + "\"";
		return *this;
	}

	Tag& Tag::close()
	{
		this->result += ">";
		return *this;
	}

	const std::string& Tag::get()
	{
		this->result += ">";
		return this->result;
	}

	// The interface

	std::unique_ptr<HtmlProvider> HtmlProvider::currentProvider = std::make_unique<DefaultHtmlProvider>();

	HtmlProvider& HtmlProvider::get()
	{
		return *currentProvider;
	}

	// Default provider

	std::pair<std::string, std::string> DefaultHtmlProvider::getBlockquote() const
	{
		return { "<blockquote>", "</blockquote>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getParagraph() const
	{
		return { "<p>", "</p>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getHeading(int h) const
	{
		std::string tag = "h" + std::to_string(h);
		return { "<" + tag + ">", "</" + tag + ">" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getCode() const
	{
		return { "<pre><code>", "</code></pre>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getOrderedList() const
	{
		return { "<ol>", "</ol>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getUnorderedList() const
	{
		return { "<ul>", "</ul>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getListItem() const
	{
		return { "<li>", "</li>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getImage(const std::string& alt, const std::string& url, const std::string& title) const
	{
		auto tag = Tag("img")
			.addattribute("src", url)
			.addattribute("alt", alt);
		if (!title.empty())
			tag.addattribute("title", title);

		return { tag.get(), ""};
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getLink(const std::string& url, const std::string& title) const
	{
		auto tag = Tag("a").addattribute("href", url);
		if (!title.empty())
			tag.addattribute("title", title);

		return { tag.get(), "</a>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getTable() const
	{
		return { "<table>", "</table>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getTableRow() const
	{
		return { "<tr>", "</tr>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getTableCell() const
	{
		return { "<td>", "</td>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getTableHeader() const
	{
		return { "<th>", "</th>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getLineBreak() const
	{
		return { "<br>", "" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getLine() const
	{
		return { "<hr>", "" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getEmphasis() const
	{
		return { "<em>", "</em>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getStrong() const
	{
		return { "<strong>", "</strong>" };
	}

	std::pair<std::string, std::string> DefaultHtmlProvider::getInlineCode() const
	{
		return { "<code>", "</code>" };
	}

	// Pretty table provider

	std::pair<std::string, std::string> PrettyTableProvider::getTable() const
	{
		return {
			Tag("table")
				.addattribute("cellpadding", "10")
				.addattribute("border", "1")
				.addattribute("rules", "all")
				.get(),
			"</table>"
		};
	}

	std::pair<std::string, std::string> PrettyTableProvider::getTableRow() const
	{
		return {
			Tag("tr")
				.addattribute("valign", "top")
				.get(),
			"</tr>"
		};
	}

	std::pair<std::string, std::string> PrettyTableProvider::getTableCell() const
	{
		return {"<td>", "</td>"};
	}

	std::pair<std::string, std::string> PrettyTableProvider::getTableHeader() const
	{
		return { "<th>", "</th>" };
	}
}