#ifndef SHIP_HPP
#define SHIP_HPP
#include <vector>
#include <stdexcept>
#include "shippart.hpp"

enum ShipDirection {
	LEFT, UP, RIGHT, DOWN
};

class Ship {
	public:
		Ship(int row, int column, int length = 1, ShipDirection direction = ShipDirection::RIGHT);
		Ship(int row, int col, CellType type);

			
		CellType applyAttack(int row, int column);
        bool isAllShipPartsAttacked() const;
		
		bool contains(int row, int column) const;
        float minimalDistanceTo(int row, int col) const;

		void kill();
        bool isKilled() const;

        void addPart(int row, int column, CellType type);
        CellType partState(int row, int column) const;
		const std::vector<ShipPart> &parts() const;
        
	private:
		std::vector<ShipPart> shipParts;
};
#endif