project(b0mb3rman)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_package(glbinding REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(lyra REQUIRED)
find_package(spdlog REQUIRED)
find_package(glfw3 REQUIRED)
find_package(freeimage REQUIRED)

add_executable(b0mb3rman 
        src/main.cpp 
        src/application.cpp
        src/game.cpp
        src/render/resource.cpp
        src/render/loader.cpp
        src/render/tileset.cpp
        src/render/tiled_map.cpp
        src/render/tile_renderer.cpp
        src/render/tile_map_renderer.cpp
        src/render/tile_program.cpp
        src/utils/io.cpp
        src/utils/json.cpp
)
target_link_libraries(b0mb3rman PUBLIC 
        bfg::lyra 
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        glfw
        glbinding::glbinding
        glbinding::glbinding-aux
        freeimage::freeimage
)
target_compile_features(b0mb3rman PRIVATE cxx_std_17)
target_include_directories(b0mb3rman PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)