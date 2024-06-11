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

    std::string getMarkdownText(const std::string& line);

    enum class ParseCode
    {
        Discard,
        ParseNext,
        ElementComplete,
        ParseNextAcceptPrevious,
        ElementCompleteDiscardPrevious,
        RequestMore,
        Invalid
    };

    enum class Type
    {
        Blank,
        Paragraph,
        NewLine,
        Heading,
        Blockquote,
        Link,
        Image,
        Table
    };

    struct ParseResult;

    struct Element
    {
        virtual Type getType() const = 0;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) = 0;
        virtual void finalize() {};

        virtual std::string getText() const { return ""; }
        virtual std::string getHtml() const { return ""; }
    };

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
}

#endif