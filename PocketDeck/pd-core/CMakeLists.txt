cmake_minimum_required(VERSION 3.31)
project(cards)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

find_package(Boost REQUIRED)

set(CMAKE_SOURCE_DIR src)
include_directories(${CMAKE_SOURCE_DIR})
add_executable(server
	src/server.cpp
	src/game.cpp
	src/games/uno.cpp
	src/main.cpp
)

find_package(Boost REQUIRED COMPONENTS json)
target_include_directories(server PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(server PRIVATE boost_system ${Boost_LIBRARIES} Boost::json)
