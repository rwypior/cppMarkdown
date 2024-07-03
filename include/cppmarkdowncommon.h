#ifndef _h_cppmarkdowncommon
#define _h_cppmarkdowncommon

#include <stdexcept>
#include <string>
#include <memory>
#include <vector>

namespace Markdown
{
    class FileException : public std::runtime_error
    {
    public:
        FileException(const std::string& what)
            : std::runtime_error(what)
        { }
    };

    template<typename T>
    struct Bitfield
    {
        T value;

        Bitfield(T value) : value(value) {}
        Bitfield(unsigned int value) : value(static_cast<T>(value)) {}
        operator T() const { return value; }
        operator bool() const { return static_cast<unsigned int>(value); }
    };

#define DEFINE_BITFIELD(Enum) \
	static Bitfield<Enum> operator |(Enum a, Enum b) { return static_cast<unsigned int>(a) | static_cast<unsigned int>(b); } \
	static Bitfield<Enum> operator &(Enum a, Enum b) { return static_cast<unsigned int>(a) & static_cast<unsigned int>(b); } \
    static Bitfield<Enum> operator |=(Enum& a, Enum b) { return a = static_cast<Enum>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b)); } \
	static Bitfield<Enum> operator &=(Enum& a, Enum b) { return a = static_cast<Enum>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b)); }

    std::string getMarkdownText(const std::string& line);

    enum class ParseCode
    {
        Discard, // Discard current component
        ParseNext, // Parse next element with current element as previous
        ElementComplete, // Add current element, and previous element if present
        ParseNextAcceptPrevious, // Parse next element and add previous element
        ElementCompleteDiscardPrevious, // Add current element and discard previous element
        RequestMore, // Parse next element and pass to current element
        Invalid // Element is invalid
    };

    enum class Type
    {
        Blank,
        Paragraph,
        Heading,
        Blockquote,
        List,
        ListItem,
        Image,
        Table,
        LineBreak,
        Code,
        Line
    };

    enum class ElementOptions
    {
        Normal = 0x00,  // No special options applied
        Raw = 0x01      // Drop HTML tags when obtaining HTML output
    };

    struct ParseResult;

    class Element
    {
    public:
        ElementOptions options = ElementOptions::Normal;
        Element* parent = nullptr;

        virtual Type getType() const = 0;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) = 0;
        virtual void finalize() {};
        unsigned int getLevel() const
        {
            unsigned int level = 0;
            Element* p = this->parent;
            while (p)
            {
                level++;
                p = p->parent;
            }
            return level;
        }

        virtual std::string getText() const { return ""; }
        virtual std::string getHtml() const { return ""; }
    };

    DEFINE_BITFIELD(ElementOptions);

    struct ParseResult
    {
        std::shared_ptr<Element> element;
        ParseCode code;

        ParseResult(
            ParseCode code = ParseCode::Invalid,
            std::shared_ptr<Element> element = nullptr
        )
            : code(code)
            , element(element)
        { }

        operator bool() const
        {
            return this->code != ParseCode::Invalid;
        }
    };

    struct Style
    {
        std::string openingTag = "";
        std::string closingTag = "";

        Style() = default;
        Style(const std::string& openingTag, const std::string& closingTag)
            : openingTag(openingTag)
            , closingTag(closingTag)
        { }
    };

    struct SpanPositionPair;

    struct MarkdownStyle
    {
        enum class SearchMode
        {
            Opening,
            Closing
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

        static MarkdownStyle makeHtml(const std::string& opening, const std::string& closing)
        {
            return MarkdownStyle("", "", opening, closing);
        }

        virtual SpanPositionPair findIn(const std::string& str, size_t offset, SearchMode mode, size_t level) const;
    };

    struct LinkStyle : public MarkdownStyle
    {
        std::string link;
        std::string text;

        virtual SpanPositionPair findIn(const std::string& str, size_t offset, SearchMode mode, size_t level) const override;
    };

    struct StyleSpan
    {
        size_t from;
        size_t to;
        size_t tagFrom;
        size_t tagTo;
        //const std::shared_ptr<MarkdownStyle> style;
        MarkdownStyle style;
        size_t level;

        //StyleSpan(size_t from, size_t to, size_t tagFrom, size_t tagTo, const std::shared_ptr<MarkdownStyle> style, size_t level)
        StyleSpan(size_t from, size_t to, size_t tagFrom, size_t tagTo, const MarkdownStyle &style, size_t level)
            : from(from)
            , to(to)
            , tagFrom(tagFrom)
            , tagTo(tagTo)
            , style(style)
            , level(level)
        { }
    };

    struct SpanPositionPair
    {
        std::shared_ptr<StyleSpan> styleSpan;
        size_t position = std::string::npos;
    };

    template<typename It>
    bool isNumber(It first, It second)
    {
        for (; first != second; first++)
            if (!std::isdigit(*first))
                return false;
        return true;
    }

    inline void ltrim(std::string& text)
    {
        text.erase(text.begin(), std::find_if(text.begin(), text.end(), [](char ch) {
            return !std::isspace(ch);
        }));
    }

    inline std::string ltrim(const std::string &text)
    {
        std::string result = text;
        ltrim(result);
        return result;
    }

    inline void rtrim(std::string& text)
    {
        text.erase(std::find_if(text.rbegin(), text.rend(), [](char ch) {
            return !std::isspace(ch);
        }).base(), text.end());
    }

    inline std::string rtrim(const std::string& text)
    {
        std::string result = text;
        rtrim(result);
        return result;
    }

    inline void trim(std::string& text)
    {
        ltrim(text);
        rtrim(text);
    }

    inline std::string trim(const std::string& text)
    {
        std::string result = text;
        ltrim(result);
        rtrim(result);
        return result;
    }
}

#endif