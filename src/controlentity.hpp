#ifndef CONTROLENTITY_HPP
#define CONTROLENTITY_HPP

#include "map.hpp"

class ControlEntity {
	public:
		ControlEntity(const std::string& name) : entityName(name) {}
		
		void printMaps(){
			std::cout << "[*] " << this->entityName << "\n";
			
			this->shipMap.printHeader();
			std::cout << "       "; 
			this->attackMap.printHeader();
			std::cout << "\n";

			for (int row = 0; row < MAPSIZE; ++row) {
				this->shipMap.printRow(row);
				std::cout << "       ";
				
				this->attackMap.printRow(row);
				std::cout << "\n";
			}
		}

		CellType applyAttackAtShipMap(int row, int column) {
			return this->shipMap.cellAttacked(row, column);
		}

		void applyAttackAtAttackMap(int row, int column, CellType type) {
			this->attackMap.applyAttack(row, column, type);
		}

		bool isAllShipsDefeated() const {
			return this->shipMap.isAllShipsDefeated();
		}

	protected:
		Map shipMap;
		Map attackMap;

	private:
		std::string entityName;
};
#endif