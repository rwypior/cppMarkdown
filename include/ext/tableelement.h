#ifndef _h_cppmarkdownext_tableelement
#define _h_cppmarkdownext_tableelement

#include "cppmarkdowncommon.h"
#include "textentry.h"

namespace Markdown
{
    class TableElement : public Element
    {
    using Cell = TextEntry;
    using Row = std::vector<Cell>;

    public:
        enum class Flags
        {
            None = 0x00,
            Started = 0x01
        };

    public:
        TableElement(const std::string& content = "");

        virtual Type getType() const override;
        virtual ParseResult parse(const std::string& line, std::shared_ptr<Element> previous) override;
        virtual ParseResult supply(const std::string& line, std::shared_ptr<Element> previous) override;

        size_t columnCount() const;
        void setColumns(size_t n);
        void setColumns(const std::initializer_list<Cell>& headerCells);

        size_t rowCount() const;
        void addRow(const std::initializer_list<Cell>& cells);

        Cell* getCell(size_t column, size_t row);
        Cell* getHeaderCell(size_t column);

        Row* getRow(size_t row);
        Row* getHeader();

        virtual std::string getText() const override;
        virtual std::string getHtml() const override;

        static bool tableLineValid(const std::string &line, size_t requiredPipes);

    protected:
        Flags flags = Flags::None   ;
        Row header;
        std::vector<Row> rows;
    };

    DEFINE_BITFIELD(TableElement::Flags);
}

#endif
