# Source: https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

file(GLOB UNITTEST_LIST "*.cpp")
add_executable(unittests 
  ${UNITTEST_LIST}
)

target_link_libraries(unittests PRIVATE 
    Catch2::Catch2WithMain
    b0mb3rman::engine
    b0mb3rman::game
)


catch_discover_tests(unittests)