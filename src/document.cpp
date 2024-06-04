#include "cppmarkdown.h"

#include <fstream>
#include <sstream>
#include <cassert>

namespace Markdown
{
	std::string getMarkdownText(const std::string& line)
	{
		if (line.empty())
			return line;

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

	ParseResult Document::parseLine(const std::string& line, std::shared_ptr<Element> previous)
	{
		if (ParseResult result = parseElement<HeadingElement>(line, previous))
			return result;

		if (ParseResult result = parseElement<ParagraphElement>(line, previous))
			return result;

		return parseElement<ParagraphElement>(line, previous);
	}

	void Document::parse(const std::string &content)
	{
        std::shared_ptr<Element> element = nullptr;
        std::istringstream str(content);
        for (std::string line; std::getline(str, line); )
        {
            //ParseResult result = element->parse(line, element);
            ParseResult result = this->parseLine(line, element);

			switch (result.code)
			{
				case ParseCode::Discard:
					break;

				case ParseCode::RequestMore:
					element = result.element;
					break;

				case ParseCode::ElementComplete:
					this->addElement(result.element);
					element = nullptr;
					break;

				case ParseCode::RequestMoreAcceptPrevious:
					this->addElement(element);
					element = result.element;
					break;

				case ParseCode::ElementCompleteDiscardPrevious:
					this->addElement(result.element);
					element = nullptr;
					break;

				case ParseCode::Invalid:
					assert(!"Invalid element");
					break;
			}
        }

		if (element)
			this->addElement(element);
	}

	void Document::addElement(std::shared_ptr<Element> element)
	{
		this->elements.push_back(element);
	}
}