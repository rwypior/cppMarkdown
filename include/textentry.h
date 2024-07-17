#ifndef _h_cppmarkdowntextentry
#define _h_cppmarkdowntextentry

#include "cppmarkdowncommon.h"

#include <string>
#include <vector>

namespace Markdown
{
    struct MarkdownStyle;

    class TextEntry
    {
    public:
        enum class HtmlOptions
        {
            Normal = 0x00,
            SkipDefaultTags = 0x01
        };

    public:
        struct Span
        {
            std::vector<std::unique_ptr<Span>> children;
            std::string text;
            std::shared_ptr<MarkdownStyle> style;
            size_t level;

            Span(const std::string& text, std::shared_ptr<MarkdownStyle> style, size_t level = 0);

            std::string getOpeningTag();
            std::string getClosingTag();

            virtual std::string getText() const;
            virtual std::string getHtml() const;
        };

        std::vector<std::unique_ptr<Span>> spans;

        //TextEntry(const std::string& content, const std::shared_ptr<MarkdownStyle> defaultStyle = std::make_shared<GenericStyle>());
        TextEntry(const std::string& content, const std::shared_ptr<MarkdownStyle> defaultStyle = nullptr);

        std::string getText() const;
        std::string getHtml(HtmlOptions flags = HtmlOptions::Normal) const;
        std::string getRawText() const;

        bool empty() const;
    };

    struct MarkdownStyle
    {
        enum class SearchMode
        {
            Opening,
            Closing
        };

        struct Result
        {
            std::unique_ptr<TextEntry::Span> span = nullptr;
            size_t position; // Position of first markdown character
            size_t length; // Length of markdown sequence

            Result(size_t position = std::string::npos, size_t length = std::string::npos, std::unique_ptr<TextEntry::Span> &&span = nullptr)
                : position(position)
                , length(length)
                , span(std::move(span))
            {
            }

            operator bool() const
            {
                return this->position != std::string::npos;
            }
        };

        std::string markdownOpening = "";
        std::string markdownClosing = "";
        Style style;
        bool acceptsSubstyles = true;
        std::vector<std::string> autoescape{};

        MarkdownStyle() = default;

        MarkdownStyle(const std::string& markdownOpening, const std::string& markdownClosing, const Style& style, bool acceptsSubstyles = true, const std::vector<std::string>& autoescape = {})
            : markdownOpening(markdownOpening)
            , markdownClosing(markdownClosing)
            , style(style)
            , acceptsSubstyles(acceptsSubstyles)
            , autoescape(autoescape)
        { }

        MarkdownStyle(const std::string& markdownOpening, const std::string& markdownClosing, const std::string& htmlOpening, const std::string& htmlClosing, bool acceptsSubstyles = true, const std::vector<std::string>& autoescape = {})
            : markdownOpening(markdownOpening)
            , markdownClosing(markdownClosing)
            , style(htmlOpening, htmlClosing)
            , acceptsSubstyles(acceptsSubstyles)
            , autoescape(autoescape)
        { }

        template<typename T = GenericStyle>
        static std::shared_ptr<T> makeHtml(const std::string& opening, const std::string& closing)
        {
            return std::make_shared<T>("", "", opening, closing);
        }

        virtual Result findIn(const std::string& str, size_t offset, SearchMode mode, size_t level) const = 0;
    };

    struct GenericStyle : public MarkdownStyle
    {
        GenericStyle() = default;

        GenericStyle(const std::string& markdownOpening, const std::string& markdownClosing, const Style& style, bool acceptsSubstyles = true, const std::vector<std::string>& autoescape = {})
            : MarkdownStyle(markdownOpening, markdownClosing, style, acceptsSubstyles, autoescape)
        { }

        GenericStyle(const std::string& markdownOpening, const std::string& markdownClosing, const std::string& htmlOpening, const std::string& htmlClosing, bool acceptsSubstyles = true, const std::vector<std::string>& autoescape = {})
            : MarkdownStyle(markdownOpening, markdownClosing, htmlOpening, htmlClosing, acceptsSubstyles, autoescape)
        { }

        virtual Result findIn(const std::string& str, size_t offset, SearchMode mode, size_t level) const override;
    };

    struct LinkStyle : public MarkdownStyle
    {
        struct LinkSpan : TextEntry::Span
        {
            std::string url;

            LinkSpan(const std::string& text, const std::string& url, std::shared_ptr<MarkdownStyle> style, size_t level = 0);

            virtual std::string getText() const override;
            virtual std::string getHtml() const override;
        };

        virtual Result findIn(const std::string& str, size_t offset, SearchMode mode, size_t level) const override;
    };

    DEFINE_BITFIELD(TextEntry::HtmlOptions);
}

#endif