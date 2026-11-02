cmake_minimum_required(VERSION 3.24)
project(Void.INC)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# ---- SFML 3 ---------------------------------------------------------------
FetchContent_Declare(
		SFML
		GIT_REPOSITORY https://github.com/SFML/SFML.git
		GIT_TAG        3.0.0
)
set(SFML_BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SFML)

# ---- Dear ImGui -----------------------------------------------------------
FetchContent_Declare(
		imgui
		GIT_REPOSITORY https://github.com/ocornut/imgui.git
		GIT_TAG        v1.91.6
)
FetchContent_MakeAvailable(imgui)

# ---- imgui-sfml (master = SFML 3 support) ---------------------------------
FetchContent_Declare(
		imgui-sfml
		GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
		GIT_TAG        master
)
set(IMGUI_DIR            ${imgui_SOURCE_DIR} CACHE PATH "" FORCE)
set(IMGUI_SFML_FIND_SFML OFF                CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(imgui-sfml)

# ---- Sources --------------------------------------------------------------
file(GLOB_RECURSE SOURCES
		"main.cpp"
		"src/*.cpp"
)

add_executable(Void.INC ${SOURCES})

target_include_directories(Void.INC PRIVATE
		"includes"
)

target_link_libraries(Void.INC PRIVATE
		sfml-graphics
		sfml-window
		sfml-system
		sfml-audio
		sfml-network
		ImGui-SFML::ImGui-SFML
)