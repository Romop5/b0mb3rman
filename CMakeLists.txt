cmake_minimum_required(VERSION 3.16)
project(b0mb3rman
 VERSION 0.0.0
 DESCRIPTION "A for-fun and for education, heavily inspired copy of Bomberman game"
 HOMEPAGE_URL "https://github.com/Romop5/b0mb3rman"
 LANGUAGES "CXX"
)

option(${PROJECT_NAME}_BUILD_UNITTESTS "Enables unittesting as a part of default build" FALSE)
option(${PROJECT_NAME}_BUILD_DOXYGEN   "Enables `make doxygen` target" FALSE)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_package(glbinding REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(lyra REQUIRED)
find_package(spdlog REQUIRED)
find_package(glfw3 REQUIRED)
find_package(freeimage REQUIRED)
find_package(Boost REQUIRED)
find_package(Freetype REQUIRED)

if(${PROJECT_NAME}_BUILD_DOXYGEN)
        find_package(Doxygen
                REQUIRED dot
                OPTIONAL_COMPONENTS mscgen dia)
endif()

add_library(engine 
        src/render/application.cpp
        src/render/resource.cpp
        src/render/loader.cpp
        src/render/tileset.cpp
        src/render/tiled_map.cpp
        src/render/tile_renderer.cpp
        src/render/tile_map_renderer.cpp
        src/render/tile_program.cpp
        src/render/font_renderer.cpp
        src/render/window.cpp
        src/utils/io.cpp
        src/utils/json.cpp
        src/utils/color.cpp
)

target_compile_features(engine PUBLIC cxx_std_17)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(engine PUBLIC 
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        glfw
        glbinding::glbinding
        glbinding::glbinding-aux
        freeimage::freeimage
        Boost::system
        freetype
)
add_library(b0mb3rman::engine ALIAS engine)

add_library(game 
        src/bm/game.cpp
        src/bm/game_controller.cpp
        src/bm/npc_controller.cpp
        src/bm/world.cpp
        src/bm/hud_manager.cpp
        src/bm/level.cpp
        src/bm/navigation_mesh.cpp
)
target_link_libraries(game PUBLIC 
        b0mb3rman::engine
)
target_include_directories(game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_library(b0mb3rman::game ALIAS game)

add_executable(b0mb3rman 
        src/main.cpp 
)

target_link_libraries(b0mb3rman PUBLIC 
        b0mb3rman::engine
        b0mb3rman::game
        bfg::lyra
)

if(${PROJECT_NAME}_BUILD_UNITTESTS)
        enable_testing()
        add_subdirectory(unittests)       
endif()

if(${PROJECT_NAME}_BUILD_DOXYGEN)
        doxygen_add_docs(doxygen)        
endif()
