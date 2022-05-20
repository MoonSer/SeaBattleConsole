#include "ship.hpp"
Ship::Ship(int row, int column, int length, ShipDirection direction) {
    for (int i = 0; i < length; ++i) {
        switch (direction) {
            case ShipDirection::LEFT:
                this->shipParts.emplace_back(row, column-i, CellType::SHIP);
                break;
            case ShipDirection::UP:
                this->shipParts.emplace_back(row-i, column, CellType::SHIP);
                break;
            case ShipDirection::RIGHT:
                this->shipParts.emplace_back(row, column+i, CellType::SHIP);
                break;
            case ShipDirection::DOWN: 
                this->shipParts.emplace_back(row+i, column, CellType::SHIP);
                break;
        }
    }
}

Ship::Ship(int row, int col, CellType type) {
    this->shipParts.emplace_back(row, col, type);
}


CellType Ship::applyAttack(int row, int column) {
    for (auto &shipPart : this->shipParts ) {
        if (shipPart.inCell(row, column)) {
            shipPart.updateState(CellType::ATTACKEDSHIP);
            if (this->isAllShipPartsAttacked()) {
                this->kill();
                return CellType::KILLEDSHIP;
            }
            return CellType::ATTACKEDSHIP;
        }
    }
    return CellType::EMPTY;
}

bool Ship::isAllShipPartsAttacked() const {
    for (auto &shipPart : this->shipParts) {
        if (!shipPart.isAttacked())
            return false;
    }
    return true;
}


bool Ship::contains(int row, int column) const {
    for (auto &shipPart : this->shipParts) {
        if (shipPart.inCell(row, column))
            return true;
    }
    return false;
}

float Ship::minimalDistanceTo(int row, int col) const {
    float minDistance = 10;
    for (auto &shipPart : this->shipParts) {
        float distance = shipPart.distanceTo(row, col);
        if ( distance < minDistance)
            minDistance = distance;
    }
    return minDistance;
}


void Ship::kill()  {
    for (auto &shipPart : this->shipParts )
        shipPart.updateState(CellType::KILLEDSHIP);
}

bool Ship::isKilled() const {
    for (auto &shipPart : this->shipParts) {
        if (shipPart.getType() != CellType::KILLEDSHIP) 
            return false;
    }
    return true;
}


void Ship::addPart(int row, int column, CellType type) {
    this->shipParts.emplace_back(row, column, type);
}

CellType Ship::partState(int row, int column) const {
    for(auto &shipPart : this->shipParts) {
        if (shipPart.getColumn() == column && shipPart.getRow() == row)
            return shipPart.getType();
    }
    throw std::runtime_error("Incorrect ship cell");
}

const std::vector<ShipPart> &Ship::parts() const {
    return this->shipParts;
}