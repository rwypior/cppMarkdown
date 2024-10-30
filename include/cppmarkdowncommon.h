#ifndef _h_cppmarkdowncommon
#define _h_cppmarkdowncommon

#include <stdexcept>
#include <string>
#include <memory>
#include <vector>

namespace Markdown
{
    static const int dumpIndent = 8;
    static const char dumpIndentChar = ' ';

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
        Bitfield<T> operator |(T b) { return static_cast<T>(static_cast<unsigned int>(value) | static_cast<unsigned int>(b)); };
        Bitfield<T> operator &(T b) { return static_cast<T>(static_cast<unsigned int>(value) & static_cast<unsigned int>(b)); };
        Bitfield<T> operator |=(T b) { return static_cast<T>(value = static_cast<T>(static_cast<unsigned int>(value) | static_cast<unsigned int>(b))); };
        Bitfield<T> operator &=(T b) { return static_cast<T>(value = static_cast<T>(static_cast<unsigned int>(value) & static_cast<unsigned int>(b))); };
    };

#define DEFINE_BITFIELD(Enum) \
	static Bitfield<Enum> operator |(Enum a, Enum b) { return static_cast<unsigned int>(a) | static_cast<unsigned int>(b); } \
	static Bitfield<Enum> operator &(Enum a, Enum b) { return static_cast<unsigned int>(a) & static_cast<unsigned int>(b); } \
    static Bitfield<Enum> operator |=(Enum& a, Enum b) { return a = static_cast<Enum>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b)); } \
	static Bitfield<Enum> operator &=(Enum& a, Enum b) { return a = static_cast<Enum>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b)); } \
    static Bitfield<Enum> operator <<(unsigned int a, Enum b) { return static_cast<Enum>(a << static_cast<unsigned int>(b)); } \
    static Bitfield<Enum> operator >>(unsigned int a, Enum b) { return static_cast<Enum>(a >> static_cast<unsigned int>(b)); }

    std::string getMarkdownText(const std::string& line);

    enum class ParseCode
    {
        Discard, // Discard current component
        ReplacePrevious, // Replace previous element with current one
        //ParseNext, // Parse next element with current element as previous
        ElementComplete, // Add current element, and previous element if present
        //ParseNextAcceptPrevious, // Parse next element and add previous element
        //ElementCompleteDiscardPrevious, // Add current element and discard previous element
        ElementCompleteParseNext, // Add current element and parse current line again
        RequestMore, // Parse next element and pass to current element
        Invalid // Element is invalid
    };

    enum class ParseFlags
    {
        None = 0x00,
        ErasePrevious = 0x01 // Erase previous element
    };
    DEFINE_BITFIELD(ParseFlags);

    enum class Type
    {
        None,
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
    DEFINE_BITFIELD(Type);

    enum class ElementOptions
    {
        Normal = 0x00,  // No special options applied
        Raw = 0x01      // Drop HTML tags when obtaining HTML output
    };
    DEFINE_BITFIELD(ElementOptions);

    std::string typeToString(Type type);

    struct Element;

    struct ParseResult
    {
        std::shared_ptr<Element> element;
        ParseCode code;
        ParseFlags flags = ParseFlags::None;

        ParseResult(ParseCode code = ParseCode::Invalid, ParseFlags flags = ParseFlags::None, std::shared_ptr<Element> element = nullptr)
            : code(code)
            , flags(flags)
            , element(element)
        { }

        operator bool() const
        {
            return this->code != ParseCode::Invalid;
        }
    };

    class Element
    {
    public:
        ElementOptions options = ElementOptions::Normal;
        Element* parent = nullptr;

        // Get element's type
        virtual Type getType() const = 0;

        // Parse initial line of the element
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) = 0;

        // Supply additional lines of the element if result of previous parse or supply function was RequestMore
        virtual ParseResult supply(const std::string& /*line*/, std::shared_ptr<Element> /*previous*/) { return ParseResult(ParseCode::Invalid); }

        // Finalize element parsing after parse and/or supply functions declared the parsing is finished
        virtual void finalize() {};

        // Get element's level in the tree structure
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
        virtual std::string dump(int indent = 0) const;
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

    inline std::string ltrimmed(const std::string& text)
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

    inline std::string rtrimmed(const std::string& text)
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

    inline std::string trimmed(const std::string& text)
    {
        std::string result = text;
        ltrim(result);
        rtrim(result);
        return result;
    }

    void replace_in(std::string &source, const std::string& target, const std::string& replacement);
    std::string replace(std::string source, const std::string& target, const std::string& replacement);

    std::string dumpElement(const Element& element, int indent = 0);
}

#endif