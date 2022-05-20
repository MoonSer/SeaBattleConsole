#include "shippart.hpp"

ShipPart::ShipPart(int r, int c, CellType t) : cell{r, c}, type(t) {}

bool ShipPart::inCell(int r, int c) const {
    return r == this->getRow() && c == this->getColumn();
}

float ShipPart::distanceTo(int r, int c) const {
    auto val = std::sqrt((this->getRow()-r)*(this->getRow()-r) + (this->getColumn()-c)*(this->getColumn()-c));
    return val;
}

void ShipPart::updateState(CellType type) {
    this->type = type;
}

bool ShipPart::isAttacked() const {
    return (this->type == CellType::ATTACKEDSHIP);
}

int ShipPart::getRow() const {
    return this->cell.row;
}

int ShipPart::getColumn() const {
    return this->cell.column;
}

CellType ShipPart::getType() const {
    return this->type;
}