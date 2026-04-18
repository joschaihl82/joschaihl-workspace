cmake_minimum_required(VERSION 3.5)

project(radionulln LANGUAGES C)

add_executable(radionulln main.c)

install(TARGETS radionulln
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
