#include "cppmarkdown.h"

#include <fstream>
#include <sstream>
#include <cassert>

namespace Markdown
{
	std::string getMarkdownText(const std::string& line)
	{
		auto pos = line.find_first_not_of(' ');
		return line.substr(pos);
	}

	Document Document::load(const std::string& path)
	{
		std::ifstream ifs(path, std::ios_base::ate);
		if (ifs.is_open())
		{
			std::stringstream buf;
			buf << ifs.rdbuf();

			Document doc;
			doc.parse(buf.str());
			return doc;
		}

		throw FileException("File " + path + " could not be opened");
	}

	void Document::parse(const std::string &content)
	{
        std::shared_ptr<Element> element = std::make_shared<BlankElement>();
        std::istringstream str(content);
        for (std::string line; std::getline(str, line); )
        {
            ParseResult result = element->parse(line, element);

			switch (result.code)
			{
				case ParseCode::ElementComplete:
					this->addElement(result.element);
					element = std::make_shared<BlankElement>();
					break;

				case ParseCode::RequestMore:
					element = result.element;
					break;

				case ParseCode::Invalid:
					assert(!"Invalid element");
					break;
			}
        }
	}

	void Document::addElement(std::shared_ptr<Element> element)
	{
		this->elements.push_back(element);
	}
}