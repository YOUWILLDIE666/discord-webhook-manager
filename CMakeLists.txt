cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project("discord-webhook" VERSION 1.0.8 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Default to Release build" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include("cmake/vcpkg.cmake")

add_executable(${PROJECT_NAME} "main.cpp")

find_package(unofficial-curlpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS filesystem system)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    unofficial::curlpp::curlpp 
    nlohmann_json::nlohmann_json 
    Boost::filesystem 
    Boost::system
)

function(set_compiler_flags target)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE 
            "$<$<CONFIG:Release>:/O3>"
            "$<$<CONFIG:Release>:/MT>"
            "$<$<CONFIG:Debug>:/Od>"
            "$<$<CONFIG:Debug>:/MTd>"
            "$<$<CONFIG:RelWithDebInfo>:/O3>"
            "$<$<CONFIG:RelWithDebInfo>:/Zi>"
            "$<$<CONFIG:MinSizeRel>:/O1>"
        )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target} PRIVATE 
            "$<$<CONFIG:Release>:-O2>"
            "$<$<CONFIG:Debug>:-O0>"
            "$<$<CONFIG:RelWithDebInfo>:-O2 -g>"
            "$<$<CONFIG:MinSizeRel>:-Os>"
            "-static"
        )
    else()
        message(WARNING "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

set_compiler_flags(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
