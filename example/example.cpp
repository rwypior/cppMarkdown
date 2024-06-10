#include "cppmarkdown.h"

#include <string>

void simpleExample()
{
	std::string markdown = R"md(# Some title)md";

	Markdown::Document doc;
	doc.parse(markdown);
}

int main()
{
	simpleExample();

	return 0;
}
