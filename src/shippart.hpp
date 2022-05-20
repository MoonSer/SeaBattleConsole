#ifndef SHIPPART_HPP
#define SHIPPART_HPP

#include "celltype.hpp"
#include "cell.hpp"
#include <cmath>

class ShipPart {
    public:
	    ShipPart(int r, int c, CellType t);

	    bool inCell(int r, int c) const;
        
        float distanceTo(int r, int c) const;

        void updateState(CellType type);

        bool isAttacked() const;

        int getRow() const;
        int getColumn() const;
        CellType getType() const;

    private:
	    Cell cell;
	    CellType type;
};

#endif