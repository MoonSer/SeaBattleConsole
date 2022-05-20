#ifndef SHIPINFO_HPP
#define SHIPINFO_HPP
#include <vector>
#include <string>

#define MAPSIZE 10	
#define SHIPSCOUNT 10

struct ShipInfo {
	std::string name;
	int count;
	int length;
};

inline std::vector<ShipInfo> AVAILABLE_SHIPS {
	{"Four - deck", 1, 4},
	{"Three - deck", 2, 3},
	{"Double deck", 3, 2},
	{"Single deck", 4, 1}
};
#endif