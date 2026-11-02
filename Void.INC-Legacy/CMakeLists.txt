cmake_minimum_required(VERSION 3.24)
project(Void.INC)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
		SFML
		GIT_REPOSITORY https://github.com/SFML/SFML.git
		GIT_TAG 3.0.0
)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SFML)

add_executable(Void.INC "Void_INC.cpp")

target_link_libraries(Void.INC sfml-graphics sfml-window sfml-system sfml-audio sfml-network)