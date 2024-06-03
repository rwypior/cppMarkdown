#include <string>
#include <memory>
#include <vector>

namespace Markdown
{
    std::string getMarkdownText(const std::string& line)
    {
        auto pos = line.find_first_not_of(' ');
        return line.substr(pos);
    }

    struct Element
    {
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

        ParseCode code;
        Type type;

        virtual std::unique_ptr<Element> parse(const std::string& line, std::unique_ptr<Element>&& previous) = 0;

        Element(Type type, ParseCode code)
            : type(type)
            , code(code)
        { }
    };

    struct BlankElement : Element
    {
        virtual std::unique_ptr<Element> parse(const std::string& line, std::unique_ptr<Element>&& previous) override
        {
            char first = *line.begin();

            if (line.rfind("# ", 0) == 0)
                return std::make_unique<Markdown::HeadingElement>(Markdown::HeadingElement::Heading::Heading1, getMarkdownText(line), Markdown::Element::ParseCode::ElementComplete);

            if (line.rfind("## ", 0) == 0)
                return std::make_unique<Markdown::HeadingElement>(Markdown::HeadingElement::Heading::Heading2, getMarkdownText(line), Markdown::Element::ParseCode::ElementComplete);

            if (line.rfind("### ", 0) == 0)
                return std::make_unique<Markdown::HeadingElement>(Markdown::HeadingElement::Heading::Heading3, getMarkdownText(line), Markdown::Element::ParseCode::ElementComplete);

            if (line.rfind("#### ", 0) == 0)
                return std::make_unique<Markdown::HeadingElement>(Markdown::HeadingElement::Heading::Heading4, getMarkdownText(line), Markdown::Element::ParseCode::ElementComplete);

            if (line.rfind("##### ", 0) == 0)
                return std::make_unique<Markdown::HeadingElement>(Markdown::HeadingElement::Heading::Heading5, getMarkdownText(line), Markdown::Element::ParseCode::ElementComplete);

            if (line.rfind("###### ", 0) == 0)
                return std::make_unique<Markdown::HeadingElement>(Markdown::HeadingElement::Heading::Heading6, getMarkdownText(line), Markdown::Element::ParseCode::ElementComplete);

            if (line.rfind("| ") != std::string::npos)
                return std::make_unique<Markdown::HeadingElement>(
                    Markdown::HeadingElement::Heading::Heading1,
                    getMarkdownText(line),
                    Markdown::Element::ParseCode::ElementComplete
                );

            return std::make_unique<Markdown::ParagraphElement>(
                getMarkdownText(line),
                Markdown::Element::ParseCode::RequestMore
            );
        }
    };

    struct ParagraphElement : Element
    {
        std::string text;

        ParagraphElement(const std::string& text, ParseCode code)
            : Element(Type::Paragraph, code)
            , text(text)
        { }
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
            Heading6
        };

        Heading heading;
        std::string text;

        HeadingElement(Heading heading, const std::string& text, ParseCode code)
            : Element(Type::Heading, code)
            , heading(heading)
            , text(text)
        { }
    };

    class Document
    {
    public:
        Document(const std::string &content)
        {

        }

        static void load(const std::string& path)
        {

        }

        void parse()
        {

        }

    private:
        std::vector<std::unique_ptr<Element>> elements;
    };
}