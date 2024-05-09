cmake_minimum_required(VERSION 3.24)
project(project C)

set(CMAKE_C_STANDARD 99)

#add_executable(fieldrunners main.c)
add_executable(fieldrunners test.c)

LINK_DIRECTORIES("./SDL2.dll")
target_link_libraries(fieldrunners mingw32)
target_link_libraries(fieldrunners SDL2main)
target_link_libraries(fieldrunners SDL2)
target_link_libraries(fieldrunners SDL2_image)
target_link_libraries(fieldrunners SDL2_ttf)

