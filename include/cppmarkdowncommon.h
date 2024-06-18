#ifndef _h_cppmarkdowncommon
#define _h_cppmarkdowncommon

#include <stdexcept>
#include <string>
#include <memory>

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
        ListElement,
        Image,
        Table,
        LineBreak
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

        virtual Type getType() const = 0;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) = 0;
        virtual void finalize() {};

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

    struct MarkdownStyle
    {
        std::string markdownOpening = "";
        std::string markdownClosing = "";
        Style style;

        MarkdownStyle() = default;

        MarkdownStyle(const std::string& markdownOpening, const std::string& markdownClosing, const Style& style)
            : markdownOpening(markdownOpening)
            , markdownClosing(markdownClosing)
            , style(style)
        { }

        MarkdownStyle(const std::string& markdownOpening, const std::string& markdownClosing, const std::string& htmlOpening, const std::string& htmlClosing)
            : markdownOpening(markdownOpening)
            , markdownClosing(markdownClosing)
            , style(htmlOpening, htmlClosing)
        { }

        static MarkdownStyle makeHtml(const std::string& opening, const std::string& closing)
        {
            return MarkdownStyle("", "", opening, closing);
        }
    };

    template<typename It>
    bool isNumber(It first, It second)
    {
        for (; first != second; first++)
            if (!std::isdigit(*first))
                return false;
        return true;
    }
}

#endif