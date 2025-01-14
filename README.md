cppMarkdown is a library aiming at parsing Markdown syntax to tree structure, and building Markdown documents from scratch. Parsed document may be iterated element by element, or simply exported to HTML.

Supported syntax
=====
The following core Markdown syntax elements are supported:
- Blockquote
- Code
- Heading
- Line break
- Line
- List
- Paragraph

In addition, an extended syntax elements are provided as extensions:
- Table

Span elements supported by `Span` class:
- Links
- Emphasis (bold, italic, bold-italic)
- Code (including double backtick)

Not supported functionality (TODO)
- Images
- Backslash escaping
- Automatic links
- Reference-style links

> **Note:** In order to use extensions provided by the library, it is required to enable standard extensions.
> The quickest way of doing so is by using `Markdown::registerStandardExtensions();` function.

Usage
=====
To begin using cppMarkdown, link your project against `cppMarkdown` library, and include `cppmarkdown.h` all-in-one header.

Parsing
-----
The simplest use case of cppMarkdown is to simply parse given Markdown code and export it to HTML code:

    std::string markdown = "...";
    Markdown::Document doc;
    doc.parse(markdown);
    std::string html = doc.getHtml();

Traversal
-----
It is also possible to traverse resulting element tree:

    void traverseContainer(Markdown::ElementContainer *container)
    {
        if (auto* el = dynamic_cast<Markdown::Element*>(container))
        {
	        // Do something with 'el'
        }
	
        traverseContainer(container);
    }
    
    Markdown::Document doc;
    doc.parse(markdown);
    
    traverseContainer(&doc);
    
Single element parsing
-----
Every Markdown element may be parsed independently from the document.

    Markdown::ListElement el(R"list(1. First
    2. Second
        1. Nested 1
        2. Nested 2
    Paragraph)list");
    std::string html = el.getHtml();

The above code will parse multi-leveled list into HTML code without usage of the Document class.

Document creation
-----
It is possible to create a Markdown document from elements opposed to source.

    Markdown::Document doc;
    auto h1 = std::make_shared<Markdown::HeadingElement>(Markdown::HeadingElement::Heading::Heading1, "This is heading");
    auto p = std::make_shared<Markdown::ParagraphElement>("And this is paragraph");
    doc.addElement(h1);
    doc.addElement(p);
    std::string html = doc.getHtml(); // Get HTML
    std::string text = doc.getText(); // Get plaintext
    std::string markdown = doc.getMarkdown(); // Get Markdown code