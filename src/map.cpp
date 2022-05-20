#include "map.hpp"

CellType Map::cellAttacked(int row, int column) {
    for (auto &ship : this->ships) {
        if (ship.contains(row, column)) {
            return ship.applyAttack(row, column);
        }
    }

    if (!this->isAlreadyInVoidAttacks(row, column))
        this->voidAttacks.emplace_back(row, column);
    
    return CellType::ATTACKED;
}

void Map::applyAttack(int row, int column, CellType type) {
    if (type == CellType::ATTACKED) {
        if (!this->isAlreadyInVoidAttacks(row, column))
            this->voidAttacks.emplace_back(row, column);
    }else {
        for (auto &ship : this->ships) {
            for (auto &part : ship.parts()) {

                if ((part.getRow() == row && abs(part.getColumn() - column) == 1) || (part.getColumn() == column && abs(part.getRow() - row) == 1) ) {
                    ship.addPart(row, column, CellType::ATTACKEDSHIP);
                    if (type == CellType::KILLEDSHIP)
                        ship.kill();
                    return;
                }
            }
        }
        this->ships.emplace_back(row, column, type);
    }
}

bool Map::isAlreadyInVoidAttacks(int row, int column) const {
    auto foundedCell = std::find(this->voidAttacks.begin(), this->voidAttacks.end(), Cell{row, column});
    if (foundedCell != this->voidAttacks.end()) {
        return true;		
    }
    return false;
}

void Map::loadFromFile(std::ifstream &f) {
    if (!f.is_open())
        throw std::runtime_error("Can't open file");

    int lineCount = 0;
    while (f.peek() != EOF) {
        int length, row, column;
        std::string direction;
        f >> length >> row >> column >> direction;
        row -= 1;
        column -= 1;
        switch(this->recognizeAndSetShipSource(row, column, direction, length)) {
            case RecognizeError::NoError: 
                break; 
            case RecognizeError::IncorrectDirection:
                throw std::runtime_error("Error file recognizig!\nAt line:" + std::to_string(lineCount) + " Incorrect direction!\n");
                break;
            case RecognizeError::IncorrectCell:
                throw std::runtime_error("Error file recognizig!\nAt line:" + std::to_string(lineCount) + " Incorrect row or column!\n");
                break;
            case RecognizeError::CellAlreadyUsed:
                throw std::runtime_error("Error file recognizig!\nAt line:" + std::to_string(lineCount) + " Cell with this direction already in use. Change another!\n");
                break;
        }
        lineCount += 1;
    }
    if (this->ships.size() != SHIPSCOUNT)
        throw std::runtime_error("Too few ships!");
}

void Map::loadFromConsole() {
    auto ship = AVAILABLE_SHIPS.begin();
    while (true) {
        if (ship == AVAILABLE_SHIPS.end())
            break;

        std::cout << "Arrange  " << ship->count << " " << ship->name << ". Your map:\n";
        this->print();

        std::cout << "Enter ship data \"ROW COLUMN DIRECTION\".\n";

        int row, col;
        std::string direction;
        std::cin >> row >> col >> direction;
        row -= 1;
        col -= 1;
        switch(this->recognizeAndSetShipSource(row, col, direction, ship->length)) {
            case RecognizeError::NoError: 
                ship->count -= 1;
                break;
            case RecognizeError::IncorrectDirection:
                std::cout << "Incorrect direction!\n";
                break;
            case RecognizeError::IncorrectCell:
                std::cout << "Incorrect row or column!\n";
                break;
            case RecognizeError::CellAlreadyUsed:
                std::cout << "Cell with this direction already in use. Change another!\n";
                break;
        }
        if (ship->count == 0)
            ++ship;
    }
}

Map::RecognizeError Map::recognizeAndSetShipSource(int row, int column, std::string_view stringDirection, int length) {
    ShipDirection direction;
    if (this->isValidCell(row, column)) {
        if (stringDirection == "right") {
            direction = ShipDirection::RIGHT;
        }else if (stringDirection == "left") {
            direction = ShipDirection::LEFT;
        }else if (stringDirection == "up") {
            direction = ShipDirection::UP;
        }else if (stringDirection == "down") {
            direction = ShipDirection::DOWN;
        } else {
            return RecognizeError::IncorrectDirection;
        }
        if (this->setShipIfCanPlace(row, column, direction, length)) {
            return RecognizeError::NoError;	
        }else{
            return RecognizeError::CellAlreadyUsed;
        }
    }
    return RecognizeError::IncorrectCell;
}

void Map::generateRandomMap() {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
    std::uniform_int_distribution<std::mt19937::result_type> randomDirection(0, 3);
    for (auto &ship:AVAILABLE_SHIPS)
        for (int shipsCount = 0; shipsCount < ship.count; ++shipsCount)
            while (!this->setShipIfCanPlace(randomMapNumber(rng), randomMapNumber(rng), ShipDirection(randomDirection(rng)), ship.length));
}

bool Map::setShipIfCanPlace(int row, int column, ShipDirection direction, int length) {
    if (!this->isValidCell(row, column))
        return false;

    switch (direction) {
        case ShipDirection::LEFT:
            if (column - length < 0) 
                return false;
            
            for (int i = 0; i < length; ++i)
                if (!this->canPlaceShipInCell(row, column-i))
                    return false;
            break;

        case ShipDirection::UP:
            if (row - length < 0)
                return false;

            for (int i = 0; i < length; ++i)
                if (!this->canPlaceShipInCell(row-i, column))
                    return false;
            break;

        case ShipDirection::RIGHT:
            if (column + length >= MAPSIZE) 
                return false;

            for (int i = 0; i < length; ++i) 
                if (!this->canPlaceShipInCell(row, column+i))
                    return false;
            break;

        case ShipDirection::DOWN:
            if (row + length >= MAPSIZE)
                return false;

            for (int i = 0; i < length; ++i)
                if (!this->canPlaceShipInCell(row+i, column))
                    return false;
            break;
    }
    this->ships.emplace_back(row, column, length, direction);
    return true;
}

bool Map::canPlaceShipInCell(int row, int column) const {
    for (auto &ship : this->ships) {
        if (ship.minimalDistanceTo(row, column) < 2)
            return false;
    }
    return true;
}

bool Map::isValidCell(int row, int column) const {
    if (row < 0 || row >= MAPSIZE || column < 0 || column >= MAPSIZE)
        return false;
    return true;
}

void Map::print() const {
    this->printHeader();
    std::cout << "\n";
    for (int i = 0; i < MAPSIZE; ++i) {
        this->printRow(i);
        std::cout << "\n";
    }
}

void Map::printHeader() const {
    std::cout << "   "; 
    for (int i = 97; i < MAPSIZE+97; ++i)
        std::cout << char(i) << " "; 
}

void Map::printRow(int row) const {
    std::cout << (row+1);

    if (row != 9) {
        std::cout << " ";
    }
    std::cout  <<"|";

    for (int column = 0; column < MAPSIZE; ++column){
        switch (this->get(row, column)) {
            case CellType::EMPTY: 
                std::cout << " ";
                break;
            case CellType::SHIP:
                std::cout << "#";
                break;
            case CellType::ATTACKEDSHIP:
                std::cout << "~";
                break;
            case CellType::ATTACKED:
                std::cout << "*";
                break;
            case CellType::KILLEDSHIP:
                std::cout << "X";
                break;
        }
        std::cout << "|";
    }
}

CellType Map::get(int row, int column) const {
    if (!this->isValidCell(row, column))
        return CellType::EMPTY;
    
    for (auto ship : this->ships) {
        if (ship.contains(row, column)) {
            return ship.partState(row, column);
        }
    }
    
    for (auto cell : this->voidAttacks) {
        if (cell.row == row && cell.column == column)
            return CellType::ATTACKED;
    }
    return CellType::EMPTY;
}

bool Map::isAllShipsDefeated() const {
    for (auto &ship : this->ships) {
        if (!ship.isKilled())
            return false;
    }
    return true;
}