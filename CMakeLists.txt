cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project("discord-webhook" VERSION 1.0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

include("cmake/vcpkg.cmake")

add_executable(${PROJECT_NAME} "main.cpp")

find_package(unofficial-curlpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::curlpp::curlpp nlohmann_json::nlohmann_json)

target_compile_options(
    ${PROJECT_NAME} PRIVATE "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:/O2>"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)