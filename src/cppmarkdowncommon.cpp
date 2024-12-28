#include "cppmarkdowncommon.h"
#include "document.h"
#include "textentry.h"

#include <unordered_map>
#include <sstream>

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

    // Element

    std::string Element::dump(int indent) const
    {
        std::string text = this->getText();
        replace_in(text, "\n", "\\n");
        replace_in(text, "\t", "\\t");

        std::string result = std::string(indent, dumpIndentChar) + "[" + typeToString(this->getType()) + "] " + text + "\n";

        return result;
    }
}