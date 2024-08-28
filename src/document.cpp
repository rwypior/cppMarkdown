#include "document.h"

#include "paragraphelement.h"
#include "linebreakelement.h"
#include "headingelement.h"
#include "blockquoteelement.h"
#include "listelement.h"
#include "codeelement.h"
#include "lineelement.h"
#include "blankelement.h"

#include <fstream>
#include <sstream>
#include <cassert>

namespace Markdown
{
	// Element container

	ParseResult ElementContainer::parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active, Type mask)
	{
		if (ParseResult result = parseElement<LineElement>(line, previous, active, mask))
			return result;
		
		if (ParseResult result = parseElement<ListElement>(line, previous, active, mask))
			return result;
		
		if (ParseResult result = parseElement<CodeElement>(line, previous, active, mask))
			return result;

		if (ParseResult result = parseElement<BlockquoteElement>(line, previous, active, mask))
			return result;

		if (ParseResult result = parseElement<HeadingElement>(line, previous, active, mask))
			return result;

		if (ParseResult result = parseElement<LineBreakElement>(line, previous, active, mask))
			return result;
		
		if (ParseResult result = parseElement<ParagraphElement>(line, previous, active, mask))
			return result;

		//return parseElement<ParagraphElement>(line, previous);
		//return parseElement<BlankElement>(line, previous);
		return ParseResult();
	}

	void ElementContainer::finalizeElement(std::shared_ptr<Element>& activeElement)
	{
		if (activeElement)
		{
			activeElement->finalize();
			this->addElement(activeElement);
			activeElement = nullptr;
		}
	}

	void ElementContainer::parse(const std::string& content, Type mask)
	{
		std::shared_ptr<Element> activeElement = nullptr;
		std::shared_ptr<Element> element = nullptr;
		std::istringstream str(content);
		for (std::string line; std::getline(str, line); )
		{
			bool retry = true;
			while (retry)
			{
				retry = false;
				ParseResult result = this->parseLine(line, element, activeElement, mask);

				if (result.flags & ParseFlags::ErasePrevious)
					this->elements.pop_back();

				switch (result.code)
				{
				case ParseCode::Discard:
					break;

				case ParseCode::ParseNext:
					this->finalizeElement(activeElement);
					element = result.element;
					break;

				case ParseCode::ElementComplete:
					this->finalizeElement(activeElement);
					if (element)
						this->addElement(element);
					this->addElement(result.element);
					element = result.element;
					element = nullptr;
					break;

				case ParseCode::ParseNextAcceptPrevious:
					this->finalizeElement(activeElement);
					this->addElement(element);
					element = result.element;
					break;

				case ParseCode::ElementCompleteDiscardPrevious:
					this->finalizeElement(activeElement);
					this->addElement(result.element);
					element = nullptr;
					break;
				
				case ParseCode::ElementCompleteParseNext:
					this->finalizeElement(activeElement);
					element = nullptr;
					retry = true;
					break;

				case ParseCode::RequestMore:
					activeElement = result.element;
					if (element)
					{
						activeElement->finalize();
						//this->addElement(element);
						element = nullptr;
					}
					break;

				case ParseCode::Invalid:
					activeElement = nullptr;
					//assert(!"Invalid element");
					break;
				}
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

	std::string ElementContainer::dump(int indent) const
	{
		std::string result;
		for (const auto& child : this->elements)
		{
			result += child->dump(indent + dumpIndent) + "\n";
		}
		return result;
	}

	void ElementContainer::addElement(std::shared_ptr<Element> element)
	{
		this->elements.push_back(element);
	}

	void ElementContainer::addElement(std::shared_ptr<Element> element, Container::const_iterator it)
	{
		this->elements.insert(it, element);
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