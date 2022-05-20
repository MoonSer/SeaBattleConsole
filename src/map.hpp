#ifndef MAP_HPP
#define MAP_HPP
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <random>
#include "shipinfo.hpp"
#include "ship.hpp"

class Map {
	public:
		enum RecognizeError {
			NoError, IncorrectDirection, IncorrectCell, CellAlreadyUsed
		};
		
		CellType cellAttacked(int row, int column);

		void applyAttack(int row, int column, CellType type);

		bool isAlreadyInVoidAttacks(int row, int column) const;
        
		void loadFromFile(std::ifstream &f);

		void loadFromConsole();

		RecognizeError recognizeAndSetShipSource(int row, int column, std::string_view stringDirection, int length);

		void generateRandomMap();

		bool setShipIfCanPlace(int row, int column, ShipDirection direction, int length);

		bool canPlaceShipInCell(int row, int column) const;

		bool isValidCell(int row, int column) const;

		void print() const;

		void printHeader() const;

		void printRow(int row) const;

		CellType get(int row, int column) const;

		bool isAllShipsDefeated() const;

	private:
		std::vector<Ship> ships;
		std::vector<Cell> voidAttacks;
};
#endif