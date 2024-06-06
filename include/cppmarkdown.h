#include <string>
#include <memory>
#include <vector>
#include <stdexcept>

namespace Markdown
{
    class FileException : public std::runtime_error
    {
    public:
        FileException(const std::string &what)
            : std::runtime_error(what)
        { }
    };

    std::string getMarkdownText(const std::string& line);

    enum class ParseCode
    {
        Discard,
        RequestMore,
        ElementComplete,
        RequestMoreAcceptPrevious,
        ElementCompleteDiscardPrevious,
        Invalid
    };

    enum class Type
    {
        Blank,
        Paragraph,
        NewLine,
        Heading,
        Link,
        Image,
        Table
    };

    struct TextEntry
    {
        enum class Style
        {
            Normal,
            Bold,
            Italic,
            BoldItalic
        };

        struct Span
        {
            std::string text;
            Style style;
            size_t level;

            Span(const std::string& text, Style style = Style::Normal, size_t level = 0)
                : text(text)
                , style(style)
                , level(level)
            { }
        };

        std::vector<Span> spans;

        TextEntry(const std::string& content);

        std::string getText() const;
        std::string getHtml() const;
        std::string getRawText() const;

        bool empty() const;
    };

    struct Element;

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

    struct Element
    {
        virtual Type getType() const = 0;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) = 0;
    };

    struct BlankElement : Element
    {
        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
    };

    struct ParagraphElement : Element
    {
        TextEntry text;

        ParagraphElement(const std::string &content = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
    };

    struct HeadingElement : Element
    {
        enum class Heading
        {
            Heading1,
            Heading2,
            Heading3,
            Heading4,
            Heading5,
            Heading6,

            Invalid
        };

        Heading heading;
        std::string text;

        HeadingElement(Heading heading = Heading::Heading1, const std::string& text = "")
            : heading(heading)
            , text(text)
        { }

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
    };

    template<typename T>
    ParseResult parseElement(const std::string& line, std::shared_ptr<Element> previous)
    {
        auto element = std::make_shared<T>();
        return element->parse(line, previous);
    }

    class Document
    {
    public:
        using ElementContainer = std::vector<std::shared_ptr<Element>>;

    public:
        static Document load(const std::string& path);
        ParseResult parseLine(const std::string& line, std::shared_ptr<Element> previous);
        void parse(const std::string& content);

        void addElement(std::shared_ptr<Element> element);

        ElementContainer::const_iterator begin();
        ElementContainer::const_iterator end();

    private:
        ElementContainer elements;
    };
}