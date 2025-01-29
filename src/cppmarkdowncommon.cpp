#include "cppmarkdown/cppmarkdowncommon.h"
#include "cppmarkdown/document.h"
#include "cppmarkdown/textentry.h"

#include <unordered_map>
#include <sstream>
#include <cassert>

namespace Markdown
{
	std::string typeToString(Type type)
	{
		std::unordered_map<Type, std::string> map{
            { Type::None, "None" },
            { Type::Blank, "Blank" },
            { Type::Paragraph, "Paragraph" },
            { Type::Heading, "Heading" },
            { Type::Blockquote, "Blockquote" },
            { Type::List, "List" },
            { Type::ListItem, "ListItem" },
            { Type::Image, "Image" },
            { Type::Table, "Table" },
            { Type::LineBreak, "LineBreak" },
            { Type::Code, "Code" },
            { Type::Line, "Line" }
		};
        auto it = map.find(type);
        if (it != map.end())
            return it->second;
        return "UNKNOWN";
	}

    void replace_in(std::string &source, const std::string& target, const std::string& replacement)
    {
        size_t index = 0;
        while (true)
        {
            index = source.find(target, index);
            if (index == std::string::npos)
                break;
            source.replace(index, target.size(), replacement);
            index += replacement.size();
        }
    }

    std::string replace(std::string source, const std::string& target, const std::string& replacement)
    {
        replace_in(source, target, replacement);
        return source;
    }

    std::vector<std::string> split(const std::string& source, char delimiter)
    {
        std::vector<std::string> result;
        size_t i = 0;
        size_t prev = 0;
        while(i != std::string::npos)
        {
            i = source.find_first_of(delimiter, prev);
            result.push_back(source.substr(prev, i - prev));
            prev = i + 1;
        }
        return result;
    }

    // Element

    std::string Element::dump(int indent) const
    {
        std::string text = this->getText();
        replace_in(text, "\n", "\\n");
        replace_in(text, "\t", "\\t");

        std::string result = std::string(indent, dumpIndentChar) + "[" + typeToString(this->getType()) + "] " + text + "\n";

        return result;
    }

    // References

    ReferenceManager* ReferenceManager::current = nullptr;

    ReferenceManager::ContextGuard::ContextGuard(ReferenceManager& manager)
    {
        ReferenceManager::current = &manager;
    }

    ReferenceManager::ContextGuard::~ContextGuard()
    {
        ReferenceManager::current = nullptr;
    }

    ReferenceManager* ReferenceManager::get()
    {
        return ReferenceManager::current;
    }

    void ReferenceManager::registerReference(const Reference& reference)
    {
        assert(current);
        this->references[reference.id] = reference;
    }

    std::optional<Reference> ReferenceManager::getReference(const std::string& name) const
    {
        auto it = this->references.find(name);
        if (it != this->references.end())
            return it->second;
        return {};
    }
}