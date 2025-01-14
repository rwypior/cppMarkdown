#include "ext/tableelement.h"

#include <sstream>

namespace Markdown
{
    TableElement::TableElement(const std::string& content)
    {
        this->parseSubelements(content);
    }

    Type TableElement::getType() const
    {
        return Type::Extension;
    }

    ParseResult TableElement::parse(const std::string& line, std::shared_ptr<Element> previous)
    {
        size_t countPipes = std::count(line.begin(), line.end(), '|');
        if (countPipes > 0)
        {
            this->setColumns(parseRow(line));
            return ParseResult(ParseCode::RequestMore);
        }

        return ParseResult(ParseCode::Discard);
    }

    ParseResult TableElement::supply(const std::string& line, std::shared_ptr<Element> previous)
    {
        if (!(this->flags & Flags::Started))
        {
            if (tableLineValid(line, this->columnCount()))
            {
                this->flags |= Flags::Started;
                return ParseResult(ParseCode::RequestMore);
            }
            else
                return ParseResult(ParseCode::Discard);
        }

        size_t parsedColumns = parseColumnCount(line);
        if (parsedColumns != this->columnCount())
        {
            if (this->rows.empty())
                return ParseResult(ParseCode::Discard);

            return ParseResult(ParseCode::ElementComplete);
        }

        this->addRow(parseRow(line));

        return ParseResult(ParseCode::RequestMore);
    }

    size_t TableElement::columnCount() const
    {
        return this->header.size();
    }

    void TableElement::setColumns(size_t n)
    {
        this->header.resize(n);
    }

    void TableElement::setColumns(const std::initializer_list<Cell>& headerCells)
    {
        this->setColumns(headerCells.size());

        size_t i = 0;
        for (const auto& Cell : headerCells)
        {
            this->header.at(i++) = *(headerCells.begin() + i);
        }
    }

    void TableElement::setColumns(const Row& row)
    {
        this->header = row;
    }

    size_t TableElement::rowCount() const
    {
        return this->rows.size();
    }

    void TableElement::addRow(const std::initializer_list<Cell>& cells)
    {
        this->rows.emplace_back();
        for (const Cell& cell : cells)
        {
            this->rows.back().push_back(cell);
        }
    }

    void TableElement::addRow(const Row& cells)
    {
        this->rows.push_back(cells);
    }

    TableElement::Cell* TableElement::getCell(size_t row, size_t column)
    {
        if (row < this->rows.size() && column < this->rows.at(row).size())
            return &this->rows.at(row).at(column);

        return nullptr;
    }

    TableElement::Cell* TableElement::getHeaderCell(size_t column)
    {
        if (column < this->header.size())
            return &this->header.at(column);

        return nullptr;
    }

    TableElement::Row* TableElement::getRow(size_t row)
    {
        if (row < this->rows.size())
            return &this->rows.at(row);

        return nullptr;
    }

    TableElement::Row* TableElement::getHeader()
    {
        return &this->header;
    }

    std::string TableElement::getText() const
    {
        std::vector<size_t> lengths(this->columnCount(), 0);
        size_t i = 0;
        for (const auto &th : this->header)
        {
            auto len = th.getText().length() + 1;
            if (len > lengths.at(i))
                lengths.at(i) = len;
            i++;
        }

        for (const auto &row : this->rows)
        {
            i = 0;
            for (const auto &td : row)
            {
                auto len = td.getText().length() + 1;
                if (len > lengths.at(i))
                    lengths.at(i) = len;
                i++;
            }
        }

        std::string result = "";

        i = 0;
        for (const auto &th : this->header)
        {
            size_t len = th.getText().length();
            result += (i > 0 ? " " : "") + th.getText() + std::string(lengths.at(i) - len, ' ') + '|';
            i++;
        }
        result.pop_back();
        result += "\n";

        i = 0;
        for (const auto& th : this->header)
        {
            result += std::string(lengths.at(i) + (i > 0 ? 1 : 0), '-') + '|';
            i++;
        }
        result.pop_back();
        result += "\n";

        for (const auto& row : this->rows)
        {
            i = 0;
            for (const auto& td : row)
            {
                size_t len = td.getText().length();
                result += (i > 0 ? " " : "") + td.getText() + std::string(lengths.at(i) - len, ' ') + '|';
                i++;
            }
            result.pop_back();
            result += "\n";
        }
        result.pop_back();

        return result;
    }

    std::string TableElement::getHtml() const
    {
        std::string result = "<table><tr>";
        for (const auto &th : this->header)
        {
            result += std::string("<th>") + th.getHtml() + "</th>";
        }
        result += "</tr>";
        for (const auto &row : this->rows)
        {
            result += "<tr>";
            for (const auto &td : row)
            {
                result += std::string("<td>") + td.getHtml() + "</td>";
            }
            result += "</tr>";
        }
        result += "</table>";
        return result;
    }

    std::string TableElement::getMarkdown() const
    {
        return this->getText();
    }


    bool TableElement::tableLineValid(const std::string &line, size_t requiredPipes)
    {
        std::string trm = trimmed(line);
        size_t pipeCount = 1;
        char last = '\0';
        for (char c : trm)
        {
            switch(c)
            {
                case '|':
                    if (last != '-')
                        return false;
                    ++pipeCount;
                    break;
                case '-':
                    break;
                default:
                    return false;
            }
            last = c;
        }

        if (trm.back() == '|')
            --pipeCount;

        return pipeCount == requiredPipes;
    }

    TableElement::Row TableElement::parseRow(const std::string& line)
    {
        auto splitted = split(line, '|');
        TableElement::Row row(splitted.size());
        size_t i = 0;
        for (std::string s : splitted)
        {
            row.at(i++) = trimmed(s);
        }

        if (row.back().empty())
            row.pop_back();

        return row;
    }

    size_t TableElement::parseColumnCount(const std::string& line)
    {
        return std::count(line.begin(), line.end(), '|') + (line.back() != '|' ? 1 : 0);
    }
}
