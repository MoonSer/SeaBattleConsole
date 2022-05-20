cmake_minimum_required(VERSION 3.5)

project(SeaBattle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SeaBattle main.cpp
            src/bot.hpp
            src/player.hpp
            src/controlentity.hpp
            src/map.hpp
            src/map.cpp
            src/ship.hpp
            src/ship.cpp
            src/shippart.hpp
            src/shippart.cpp
            src/shipinfo.hpp
            src/celltype.hpp
            src/cell.hpp
)

target_link_libraries(SeaBattle -static)