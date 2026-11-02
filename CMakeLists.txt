cmake_minimum_required(VERSION 3.20)
project(chess LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_options(-Wall -Wextra)

# Rules and game logic: no SFML in here
add_library(chesslib src/board.cpp )
target_include_directories(chesslib PUBLIC src)

find_package(SFML 3 COMPONENTS Graphics REQUIRED)
add_executable(chess src/main.cpp src/ui.cpp)
target_link_libraries(chess PRIVATE chesslib SFML::Graphics)

include(FetchContent)
FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.5.2)
FetchContent_MakeAvailable(Catch2)
enable_testing()
add_executable(tests tests/test_board.cpp)
target_link_libraries(tests PRIVATE chesslib Catch2::Catch2WithMain)
add_test(NAME tests COMMAND tests)