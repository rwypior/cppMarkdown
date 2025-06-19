#ifndef _h_cppmarkdownhtml
#define _h_cppmarkdownhtml

#include "cppmarkdown/cppmarkdowncommon.h"

#include <string>
#include <utility>
#include <memory>

namespace Markdown
{
	// Very simple HTML tag builder
	struct Tag
	{
		std::string result;

		Tag(const std::string& name);
		Tag& add(const std::string& name);
		Tag& addattribute(const std::string& name, const std::string& value);
		Tag& close();
		const std::string& get();
	};

	// Provides an interface for a collection of HTML tags for various Markdown elements
	class HtmlProvider
	{
		template<typename FT, typename ...FParams> friend class SubstituteHtmlProvider;

	public:
		virtual ~HtmlProvider() = default;
		virtual std::pair<std::string, std::string> getBlockquote() const = 0;
		virtual std::pair<std::string, std::string> getParagraph() const = 0;
		virtual std::pair<std::string, std::string> getHeading(int h) const = 0;
		virtual std::pair<std::string, std::string> getCode() const = 0;
		virtual std::pair<std::string, std::string> getOrderedList() const = 0;
		virtual std::pair<std::string, std::string> getUnorderedList() const = 0;
		virtual std::pair<std::string, std::string> getListItem() const = 0;
		virtual std::pair<std::string, std::string> getImage(const std::string& alt, const std::string& url, const std::string& title = "") const = 0;
		virtual std::pair<std::string, std::string> getLink(const std::string& url, const std::string& title = "") const = 0;
		virtual std::pair<std::string, std::string> getTable() const = 0;
		virtual std::pair<std::string, std::string> getTableRow() const = 0;
		virtual std::pair<std::string, std::string> getTableCell() const = 0;
		virtual std::pair<std::string, std::string> getTableHeader() const = 0;
		virtual std::pair<std::string, std::string> getLineBreak() const = 0;
		virtual std::pair<std::string, std::string> getLine() const = 0;
		virtual std::pair<std::string, std::string> getEmphasis() const = 0;
		virtual std::pair<std::string, std::string> getStrong() const = 0;
		virtual std::pair<std::string, std::string> getInlineCode() const = 0;

		static HtmlProvider& get();

	private:
		static std::unique_ptr<HtmlProvider> currentProvider;
	};

	// Default collection of Markdown HTML tags
	class DefaultHtmlProvider : public HtmlProvider
	{
	public:
		virtual std::pair<std::string, std::string> getBlockquote() const override;
		virtual std::pair<std::string, std::string> getParagraph() const override;
		virtual std::pair<std::string, std::string> getHeading(int h) const override;
		virtual std::pair<std::string, std::string> getCode() const override;
		virtual std::pair<std::string, std::string> getOrderedList() const override;
		virtual std::pair<std::string, std::string> getUnorderedList() const override;
		virtual std::pair<std::string, std::string> getListItem() const override;
		virtual std::pair<std::string, std::string> getImage(const std::string& alt, const std::string& url, const std::string& title = "") const override;
		virtual std::pair<std::string, std::string> getLink(const std::string& url, const std::string& title = "") const override;
		virtual std::pair<std::string, std::string> getTable() const override;
		virtual std::pair<std::string, std::string> getTableRow() const override;
		virtual std::pair<std::string, std::string> getTableCell() const override;
		virtual std::pair<std::string, std::string> getTableHeader() const override;
		virtual std::pair<std::string, std::string> getLineBreak() const override;
		virtual std::pair<std::string, std::string> getLine() const override;
		virtual std::pair<std::string, std::string> getEmphasis() const override;
		virtual std::pair<std::string, std::string> getStrong() const override;
		virtual std::pair<std::string, std::string> getInlineCode() const override;
	};

	class PrettyTableProvider : public DefaultHtmlProvider
	{
	public:
		virtual std::pair<std::string, std::string> getTable() const override;
		virtual std::pair<std::string, std::string> getTableRow() const override;
		virtual std::pair<std::string, std::string> getTableCell() const override;
		virtual std::pair<std::string, std::string> getTableHeader() const override;
	};

	// Allows user to change HTML provider for the duration of this class instance lifetime
	template<typename T, typename ...Params>
	class SubstituteHtmlProvider
	{
	public:
		SubstituteHtmlProvider()
		{
			this->previousProvider = std::move(HtmlProvider::currentProvider);
			HtmlProvider::currentProvider = std::make_unique<T>(Params...);
		}

		~SubstituteHtmlProvider()
		{
			HtmlProvider::currentProvider = std::move(this->previousProvider);
		}

	private:
		std::unique_ptr<HtmlProvider> previousProvider = nullptr;
	};
}

#endif