cmake_minimum_required(VERSION 3.15)
project(fterminal C)

set(CMAKE_C_STANDARD 99)

add_executable(fterminal main.c hacking.h configuration.h utilities.h hacking.c configuration.c utilities.c shell.c shell.h preferences.h)

link_directories(/usr/include/)
target_link_libraries(fterminal ncurses config)