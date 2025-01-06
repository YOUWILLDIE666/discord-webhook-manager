cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project("discord-webhook" VERSION 1.0.9 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Default to Release build" FORCE)
endif()

# don't use Boost filesystem by default
option(USEBOOST "Use boost-filesystem library" OFF)

# don't static-link by default
# don't use this option by default YOU DUMBFUCKING IDIOT
# option(STATICLNK "Statick linkage (no multi-threading)" OFF)

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
)

if (USEBOOST)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USEBOOST)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Boost::filesystem
        Boost::system
    )
endif()

function(set_compiler_flags target)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE 
            "$<$<CONFIG:Release>:/O3>"
            "$<$<CONFIG:Release>:/MT>"
            # "$<$<CONFIG:Release>:/DEBUG:NO>"
            "$<$<CONFIG:Debug>:/Od>"
            "$<$<CONFIG:Debug>:/Zi>"
            "$<$<CONFIG:RelWithDebInfo>:/O2>"
            "$<$<CONFIG:RelWithDebInfo>:/Zi>"
            "$<$<CONFIG:MinSizeRel>:/O1>"
            "$<$<CONFIG:MinSizeRel>:/MT>"
            # "$<$<CONFIG:MinSizeRel>:/DEBUG:NO>"
            # i am going to eat my liver
        )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target} PRIVATE 
            "$<$<CONFIG:Release>:-O4 -DNDEBUG>"
            "$<$<CONFIG:Debug>:-O0 -g>"
            "$<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>"
            "$<$<CONFIG:MinSizeRel>:-Os -DNDEBUG>"
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
