cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(ArchipelagoGD VERSION 1.0.0)

# Add all source files inside src (recursively)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})

# target_include_directories(${PROJECT_NAME} PRIVATE
#     "${CMAKE_CURRENT_SOURCE_DIR}/../APCpp"
# )

target_include_directories(ArchipelagoGD PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# target_link_libraries(${PROJECT_NAME}
#     "${CMAKE_CURRENT_SOURCE_DIR}/../APCpp/build/libAPCpp.a"
# )

CPMAddPackage(
    NAME APCpp-for-gd
    GIT_REPOSITORY https://github.com/JellyMoose27/APCpp-for-gd
    GIT_TAG main
)

target_link_libraries(${PROJECT_NAME} APCpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${APCpp_SOURCE_DIR})