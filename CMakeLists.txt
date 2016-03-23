cmake_minimum_required(VERSION 3.3)
project(echo_server)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/osboxes/ClionProjects/echo_server")

set(CMAKE_CC_FLAGS "${CMAKE_CC_FLAGS} -std=c99")

set(SOURCE_FILES_SERVER server.c)
set(SOURCE_FILES_CLIENT client.c)
add_executable(echo_server ${SOURCE_FILES_SERVER})
add_executable(echo_client ${SOURCE_FILES_CLIENT})