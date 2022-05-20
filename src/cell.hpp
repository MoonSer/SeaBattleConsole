#ifndef CELL_HPP
#define CELL_HPP

struct Cell {
    Cell(int r, int c) : row(r), column(c) {}
    friend bool operator==(const Cell& cell1, const Cell& cell2) {
        return cell1.row == cell2.row && cell1.column == cell2.column;
    }
    int row;
    int column;
};

#endif