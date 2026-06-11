cmake_minimum_required(VERSION 3.20)
project(CCBridge)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(CCBridge
    src/main.cpp
    src/net/Server.cpp
    src/libs/imgui/imgui.cpp
    src/libs/imgui/imgui_demo.cpp
    src/libs/imgui/imgui_draw.cpp
    src/libs/imgui/imgui_tables.cpp
    src/libs/imgui/imgui_widgets.cpp
    src/libs/rlImGui/rlImGui.cpp
)

target_include_directories(CCBridge PRIVATE
    src/
    src/net/
    src/net/turtle/
    src/net/peripherals/
    src/registry/
    src/util/
    src/nodes/
    src/world/
    src/libs/imgui/
    src/libs/rlImGui/
    src/libs/crow/
    src/libs/asio/
    src/libs/nlohmann/
)

target_include_directories(CCBridge PRIVATE
    src/
    src/net/
    src/turtle/
    src/turtle/peripherals/
    src/registry/
    src/util/
    src/nodes/
    src/world/
    src/libs/imgui/
    src/libs/rlImGui/
    src/libs/crow/
    src/libs/asio/
    src/libs/nlohmann/
)

target_compile_definitions(CCBridge PRIVATE
    _WIN32_WINNT=0x0601
)

target_link_libraries(CCBridge PRIVATE
    raylib psapi ws2_32 mswsock
)

add_custom_command(TARGET CCBridge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
)
 
message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}")