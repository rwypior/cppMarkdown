#include "cppmarkdown.h"

int main()
{
	std::string markdown = R"md(# Some title
Some text
)md";

	Markdown::Document doc;
	doc.parse(markdown);

	return 0;
}
