#include <iostream>

using namespace std;

#define MAPSIZE 10

void printMap(bool map[MAPSIZE][MAPSIZE]) {
	cout << " а б в г д е ж с и к \n";
	for (int i = 0; i < MAPSIZE; ++i) {
		cout << i+1 << (i == 9 ? "" : " ") <<"|";
		for (int j = 0; j < MAPSIZE; ++j){
			cout << (map[i][j] ? "#" : " ") << "|";
		}
		cout << "\n";
	}
}

void clear(bool map[MAPSIZE][MAPSIZE]) {
	for (int i = 0; i < MAPSIZE; ++i) {
		for (int j = 0; j < MAPSIZE; ++j) {
			map[i][j] = false;
		}
	}
}

int main() {
	// Строка - название корабля,
	// первое число пары - количество кораблей,
	// Второе число пары - длина корабля
	std::map<std::string, std::pair<int, int>>> ships = {
		{"Четырёхпалубник", {1, 4}},
		{"Трёхпалубник", {2, 3}},
		{"Двухпалубник", {3, 2}},
		{"Однопалубник", {4, 1}}
	};

	cout << "                     _______. _______      ___       " << "\n"
		 << "                    /       ||   ____|    /   \\     " << "\n"
		 << "                   |   (----`|  |__      /  ^  \\    " << "\n"
		 << "                    \\   \\    |   __|    /  /_\\  \\" << "\n"
		 << "                .----)   |   |  |____  /  _____  \\  " << "\n"
		 << "                |_______/    |_______|/__/     \\__\\" << "\n";

	cout << "  .______        ___      .___________..___________. __       _______ " << "\n"
		 << "  |   _  \\      /   \\     |           ||           ||  |     |   ____|" << "\n"
		 << "  |  |_)  |    /  ^  \\    `---|  |----``---|  |----`|  |     |  |__   " << "\n"
		 << "  |   _  <    /  /_\\  \\       |  |         |  |     |  |     |   __|  " << "\n"
		 << "  |  |_)  |  /  _____  \\      |  |         |  |     |  `----.|  |____  " << "\n"
		 << "  |______/  /__/     \\__\\     |__|         |__|     |_______||_______|" << "\n";

	bool playerMap[MAPSIZE][MAPSIZE];
	clear(playerMap);

	bool botMap[MAPSIZE][MAPSIZE];
	clear(botMap);



	return 0;
}
