#include "cppmarkdown/document.h"
#include "cppmarkdown/extensions.h"

#include "cppmarkdown/paragraphelement.h"
#include "cppmarkdown/linebreakelement.h"
#include "cppmarkdown/headingelement.h"
#include "cppmarkdown/blockquoteelement.h"
#include "cppmarkdown/listelement.h"
#include "cppmarkdown/codeelement.h"
#include "cppmarkdown/lineelement.h"
#include "cppmarkdown/blankelement.h"
#include "cppmarkdown/referenceelement.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <functional>

namespace Markdown
{
	// Element container

	ParseResult ElementContainer::parseLine(const std::string& line, std::shared_ptr<Element> previous, std::shared_ptr<Element> active, Type mask)
	{
		ParserCollection parsers {
			&parseElement<ReferenceElement>,
			&parseElement<LineElement>,
			&parseElement<ListElement>,
			&parseElement<CodeElement>,
			&parseElement<BlockquoteElement>,
			&parseElement<HeadingElement>,
			&parseElement<LineBreakElement>,
			&parseElement<ParagraphElement>,
		};

		ExtensionsManager& extensions = ExtensionsManager::getInstance();
		extensions.extend(parsers);

		for (auto pred : parsers)
		{
			if (ParseResult result = pred(line, previous, active, mask))
				return result;
		}

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
		std::shared_ptr<Element> activeElement = nullptr; // Element which requested more lines
		std::shared_ptr<Element> previousElement = nullptr; // Currently parsed element
		std::shared_ptr<Element> lastElement = nullptr; // Currently parsed element

		ParseResult result;

		std::istringstream str(content);
		for (std::string line; std::getline(str, line); )
		{
			bool retry = true;
			while (retry)
			{
				retry = false;
				result = this->parseLine(line, previousElement, activeElement, mask);

				if (result.flags & ParseFlags::ErasePrevious)
				{
					previousElement = nullptr;
					this->elements.pop_back();
				}

				switch (result.code)
				{
				case ParseCode::Discard:
					break;

				case ParseCode::ReplacePrevious:
					this->finalizeElement(activeElement);
					activeElement = nullptr;

					this->elements.back() = result.element;
					break;

				case ParseCode::ElementComplete:
					this->finalizeElement(activeElement);
					activeElement = nullptr;
					this->addElement(result.element);
					break;
				
				case ParseCode::ElementCompleteParseNext:
					this->finalizeElement(activeElement);
					activeElement = nullptr;
					retry = true;
					break;

				case ParseCode::RequestMore:
					activeElement = result.element;
					break;

				case ParseCode::Invalid:
					this->finalizeElement(activeElement);
					activeElement = nullptr;
					break;
				}

				if (result.element)
					previousElement = result.element;
			}
		}

		if (activeElement)
		{
			activeElement->finalize();
			this->addElement(activeElement);
		}
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

	void ElementContainer::iterate(std::function<void(ElementContainer&, Container::iterator, const Element*, const Element*)> pred)
	{
		if (this->elements.empty())
		{
			pred(*this, this->elements.begin(), nullptr, nullptr);
			return;
		}

		for (size_t i = 0; i < this->elements.size(); i++)
		{
			Element* prev = nullptr;
			Element* next = nullptr;

			if (i > 0)
				prev = this->elements.at(i - 1).get();

			if (i < this->elements.size())
				next = this->elements.at(i).get();

			size_t size = this->elements.size();
			pred(*this, this->elements.begin() + i, prev, next);
			size_t sizeAfter = this->elements.size();

			i += sizeAfter - size;
		}
	}

	void ElementContainer::eraseElement(Container::const_iterator it)
	{
		this->elements.erase(it);
	}

	void ElementContainer::replaceElement(Container::iterator it, std::shared_ptr<Element> replacement)
	{
		*it = replacement;
	}

	void ElementContainer::replaceElements(std::function<bool(const Element&)> pred, std::function<std::shared_ptr<Element>(const Element&)> replacementPred)
	{
		for (auto it = this->elements.begin(); it != this->elements.end(); it++)
		{
			auto& el = *it->get();
			if (pred(el))
				this->replaceElement(it, replacementPred(el));
		}
	}

	void ElementContainer::replaceElements(Type type, std::function<std::shared_ptr<Element>(const Element&)> replacementPred)
	{
		this->replaceElements([type](const Element& el) {
			return el.getType() == type;
		}, replacementPred);
	}

	bool ElementContainer::empty() const
	{
		return this->elements.empty();
	}

	void ElementContainer::clear()
	{
		this->elements.clear();
	}

	size_t ElementContainer::size() const
	{
		return this->elements.size();
	}

	std::shared_ptr<Element> ElementContainer::at(size_t index) const
	{
		return this->elements.at(index);
	}

	std::shared_ptr<Element> ElementContainer::take(size_t index)
	{
		auto el = this->elements.at(index);
		this->elements.erase(this->elements.begin() + index);
		return el;
	}

	ElementContainer::Container::value_type ElementContainer::front() const
	{
		return this->elements.front();
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

	void Document::parse(const std::string& content, Type mask)
	{
		ReferenceManager::ContextGuard cg(this->referenceManager);
		ElementContainer::parse(content, mask);
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
		std::string result = "<!DOCTYPE html><html><head>";
		if (this->addCharset)
			result += "<meta charset=\"utf-8\">";
		result += "</head><body>";
		for (const auto &el : *this)
		{
			result += el->getHtml();
		}
		result += "</body></html>";
		return result;
	}

	// Subelement parser

	void SubelementParser::parseSubelements(const std::string& source)
	{
		if (!source.empty())
		{
			bool supply = false;

			std::istringstream str(source);
			for (std::string line; std::getline(str, line); )
			{
				if (supply)
				{
					this->supply(line, nullptr);
				}
				else
				{
					ParseResult result = this->parse(line, nullptr);

					if (result.code == ParseCode::RequestMore)
						supply = true;
					else
						break;
				}
			}

			this->finalize();
		}
	}

	void SubelementUnpacker::unpackTo(SubelementUnpacker& target)
	{
		ElementContainer& container = this->getContainer();

		for (auto element : container)
		{
			target.getContainer().addElement(element);
		}

		container.clear();
	}

	void SubelementUnpacker::unpackToText(Type mask)
	{
		ElementContainer& container = this->getContainer();

		const auto& element = container.front();
		if ((1 << element->getType()) & mask)
			return;

		TextEntry& text = this->getText();

		text.parse(element->getText());
		container.clear();
	}

	void SubelementUnpacker::unpackElement(ElementContainer::Container::const_iterator it)
	{
		auto element = *it;

		TextEntry& text = this->getText();

		text.parse(element->getText());

		this->getContainer().eraseElement(it);
	}
}
