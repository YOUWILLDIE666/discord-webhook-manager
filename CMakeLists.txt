cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(DiscordWebhook VERSION 1.0.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystem/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

add_executable(${PROJECT_NAME} main.cpp)

find_package(unofficial-curlpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::curlpp::curlpp nlohmann_json::nlohmann_json)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
