#include "document.h"

#include "paragraphelement.h"
#include "headingelement.h"
#include "blockquoteelement.h"
#include "listelement.h"

#include <fstream>
#include <sstream>
#include <cassert>

namespace Markdown
{
	// Element container

	ParseResult ElementContainer::parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active)
	{
		if (ParseResult result = parseElement<ListElement>(line, previous, active))
			return result;

		if (ParseResult result = parseElement<BlockquoteElement>(line, previous, active))
			return result;

		if (ParseResult result = parseElement<HeadingElement>(line, previous, active))
			return result;

		if (ParseResult result = parseElement<ParagraphElement>(line, previous, active))
			return result;

		return parseElement<ParagraphElement>(line, previous);
	}

	void ElementContainer::parse(const std::string& content)
	{
		std::shared_ptr<Element> activeElement = nullptr;
		std::shared_ptr<Element> element = nullptr;
		std::istringstream str(content);
		for (std::string line; std::getline(str, line); )
		{
			ParseResult result = this->parseLine(line, element, activeElement);

			switch (result.code)
			{
			case ParseCode::Discard:
				break;

			case ParseCode::ParseNext:
				if (activeElement)
				{
					activeElement->finalize();
					this->addElement(activeElement);
					activeElement = nullptr;
				}
				element = result.element;
				break;

			case ParseCode::ElementComplete:
				if (activeElement)
				{
					activeElement->finalize();
					this->addElement(activeElement);
					activeElement = nullptr;
				}
				if (element)
					this->addElement(element);
				this->addElement(result.element);
				element = nullptr;
				break;

			case ParseCode::ParseNextAcceptPrevious:
				if (activeElement)
				{
					activeElement->finalize();
					this->addElement(activeElement);
					activeElement = nullptr;
				}
				this->addElement(element);
				element = result.element;
				break;

			case ParseCode::ElementCompleteDiscardPrevious:
				if (activeElement)
				{
					activeElement->finalize();
					this->addElement(activeElement);
					activeElement = nullptr;
				}
				this->addElement(result.element);
				element = nullptr;
				break;

			case ParseCode::RequestMore:
				activeElement = result.element;
				if (element)
				{
					activeElement->finalize();
					this->addElement(element);
					element = nullptr;
				}
				break;

			case ParseCode::Invalid:
				activeElement = nullptr;
				assert(!"Invalid element");
				break;
			}
		}

		if (activeElement)
		{
			activeElement->finalize();
			this->addElement(activeElement);
		}

		if (element)
			this->addElement(element);
	}

	void ElementContainer::addElement(std::shared_ptr<Element> element)
	{
		this->elements.push_back(element);
	}

	bool ElementContainer::empty() const
	{
		return this->elements.empty();
	}

	ElementContainer::Container::value_type ElementContainer::back() const
	{
		return this->elements.back();
	}

	size_t ElementContainer::elementsCount() const
	{
		return this->elements.size();
	}

	ElementContainer::Container::iterator ElementContainer::begin()
	{
		return this->elements.begin();
	}

	ElementContainer::Container::iterator ElementContainer::end()
	{
		return this->elements.end();
	}

	ElementContainer::Container::const_iterator ElementContainer::begin() const
	{
		return this->elements.begin();
	}

	ElementContainer::Container::const_iterator ElementContainer::end() const
	{
		return this->elements.end();
	}

	// Document

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

	std::string Document::getText() const
	{
		std::string result;
		for (const auto &el : *this)
		{
			result += el->getText() + "\n";
		}
		
		if (!result.empty())
			result.pop_back();

		return result;
	}
	
	std::string Document::getHtml() const
	{
		std::string result = "<!DOCTYPE html><html><head></head><body>";
		for (const auto &el : *this)
		{
			result += el->getHtml();
		}
		result += "</body></html>";
		return result;
	}
}