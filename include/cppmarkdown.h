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
        RequestMore,
        ElementComplete,
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
        std::string text;

        ParagraphElement(const std::string& text = "")
            : text(text)
        { }

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
        static Document load(const std::string& path);
        void parse(const std::string& content);

        void addElement(std::shared_ptr<Element> element);

    private:
        std::vector<std::shared_ptr<Element>> elements;
    };
}