#ifndef _h_cppmarkdowncommon
#define _h_cppmarkdowncommon

#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <optional>

namespace Markdown
{
    static const int dumpIndent = 8;
    static const char dumpIndentChar = ' ';

    class ElementContainer;
    class TextEntry;

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
        Bitfield<T> operator ~() { return static_cast<T>(~static_cast<unsigned int>(value)); };
    };

// Enable bitwise operations on enum class
// Best to use on namespace-level
#define DEFINE_BITFIELD(Enum) \
	static inline Bitfield<Enum> operator |(Enum a, Enum b) { return static_cast<unsigned int>(a) | static_cast<unsigned int>(b); } \
	static inline Bitfield<Enum> operator &(Enum a, Enum b) { return static_cast<unsigned int>(a) & static_cast<unsigned int>(b); } \
    static inline Bitfield<Enum> operator |=(Enum& a, Enum b) { return a = static_cast<Enum>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b)); } \
	static inline Bitfield<Enum> operator &=(Enum& a, Enum b) { return a = static_cast<Enum>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b)); } \
    static inline Bitfield<Enum> operator <<(unsigned int a, Enum b) { return static_cast<Enum>(a << static_cast<unsigned int>(b)); } \
    static inline Bitfield<Enum> operator >>(unsigned int a, Enum b) { return static_cast<Enum>(a >> static_cast<unsigned int>(b)); } \
    static inline Bitfield<Enum> operator ~(Enum a) { return static_cast<Enum>(~static_cast<unsigned int>(a)); }

    std::string getMarkdownText(const std::string& line);

    enum class ParseCode
    {
        Discard, // Discard current component
        ReplacePrevious, // Replace previous element with current one
        ElementComplete, // Add current element, and previous element if present
        ElementCompleteParseNext, // Add current element and parse current line again
        RequestMore, // Parse next element and pass to current element
        Invalid // Element is invalid
    };

    enum class ParseFlags
    {
        None = 0x00,
        ErasePrevious = 0x01, // Erase previous element
        ForceAdd = 0x02 // Add element at RequestMore code
    };
    DEFINE_BITFIELD(ParseFlags);

    enum class FinalizeAction
    {
        None = 0x00,
        ErasePrevious = 0x01,
        Continue = 0x02,
    };
    DEFINE_BITFIELD(FinalizeAction);

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
        Line,
        Reference,
        Extension
    };
    DEFINE_BITFIELD(Type);

    std::string typeToString(Type type);

    class Element;

    struct ParseResult
    {
        ParseCode code;
        ParseFlags flags = ParseFlags::None;
        std::shared_ptr<Element> element;

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
        Element* parent = nullptr;

        virtual ~Element() = default;

        // Get element's type
        virtual Type getType() const = 0;

        // Parse initial line of the element
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) = 0;

        // Supply additional lines of the element if result of previous parse or supply function was RequestMore
        virtual ParseResult supply(const std::string& /*line*/, std::shared_ptr<Element> /*previous*/) { return ParseResult(ParseCode::Invalid); }

        // Finalize element parsing after parse and/or supply functions declared the parsing is finished
        virtual void finalize() {};

        // Finalize element after the whole document has been parsed
        virtual FinalizeAction documentFinalize(std::shared_ptr<Element> previous) { return FinalizeAction::None; };

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
        virtual std::string getInnerHtml() const { return this->getHtml(); }
        virtual std::string getMarkdown() const { return ""; }
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

    struct Reference
    {
        Reference(const std::string& id = "", const std::string& value = "", const std::string& title = "")
            : id(id)
            , value(value)
            , title(title)
        {
        }

        std::string id;
        std::string value;
        std::string title;
    };

    class ReferenceManager
    {
    public:
        struct ContextGuard
        {
            ContextGuard(ReferenceManager &manager);
            ~ContextGuard();
        };
        friend struct ContextGuard;

    public:
        static ReferenceManager* get();

        void registerReference(const Reference& reference);
        std::optional<Reference> getReference(const std::string &name) const;

    private:
        std::unordered_map<std::string, Reference> references;
        static ReferenceManager* current;
    };

    using ParserPredicate = std::function<ParseResult(const std::string&, std::shared_ptr<Element>, std::shared_ptr<Element>, Type mask)>;
    using ParserCollection = std::vector<ParserPredicate>;

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
    std::vector<std::string> split(const std::string& source, char delimiter);

    // Check if character at given position is escaped with backslash
    bool isEscaped(const std::string& str, size_t position);
}

#endif
