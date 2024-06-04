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

void exampleParagraphMixedStyles()
{
	std::string markdown = R"md(*long italic text with **bold** in it*)md";

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
	exampleParagraphMixedStyles();

	return 0;
}
