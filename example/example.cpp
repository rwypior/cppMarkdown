#include "cppmarkdown.h"

#include <string>

void exampleHeading()
{
	std::string markdown = R"md(# Some title)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleHeadingAlternate()
{
	std::string markdown = R"md(Some title
=====
Some text)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleParagraphs()
{
	std::string markdown = R"md(First paragraph
Second paragraph)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleHeadingAdvanced()
{
	std::string markdown = R"md(First title
=====
Some text
More text
Another title
===
And last text)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleBlankLines()
{
	std::string markdown = R"md(Line 1
Line 2

Line 3


Line 4






Line 5)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleParagraphStyles()
{
	//std::string markdown = R"md(Regular style **bold** regular again *this is italic* and **bold again**)md";
	std::string markdown = R"md(*italic* **bold**)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleParagraphMiddle()
{
	std::string markdown = R"md(normal **bold** end)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleParagraphMixedStyles()
{
	std::string markdown = R"md(begin *long italic text with **bold** in it* end)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleParagraphMixedStylesMultiple()
{
	std::string markdown = R"md(begin *italic **bold** italic again **bold again** third italic* end)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleParagraphMixedStylesMultipleNested()
{
	std::string markdown = R"md(begin *italic **bold *italic* again** italic* end)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleParagraphBolditalic()
{
	std::string markdown = R"md(begin **_bolditalic_** end)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

void exampleTraverse()
{
	std::string markdown = R"md(begin **_bolditalic_** end)md";

	Markdown::Document doc;
	doc.parse(markdown);

	for (auto el : doc)
	{
		if (auto para = std::dynamic_pointer_cast<Markdown::ParagraphElement>(el))
		{
			std::string html = para->text.getHtml();
		}
	}
}

void exampleTextEntry()
{
	std::string markdown = R"md(some text)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

int main()
{
	//exampleHeading();
	//exampleParagraphs
	//exampleHeadingAlternate();
	//exampleHeadingAdvanced();
	//exampleBlankLines();
	//exampleParagraphStyles();
	//exampleParagraphMiddle();
	//exampleParagraphMixedStyles();
	//exampleParagraphMixedStylesMultiple();
	//exampleParagraphMixedStylesMultipleNested();
	//exampleParagraphBolditalic();
	//exampleTraverse();
	exampleTextEntry();

	return 0;
}
