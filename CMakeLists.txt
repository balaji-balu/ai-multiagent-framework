cmake_minimum_required(VERSION 3.15)

project(ai-agent-framework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- FetchContent for Dependencies ---
include(FetchContent)

# SObjectizer
FetchContent_Declare(
    SObjectizer
    GIT_REPOSITORY https://github.com/Stiffstream/sobjectizer.git
    GIT_TAG v.5.8.4
)

# uWebSockets
FetchContent_Declare(
    uwebsockets
    GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
    GIT_TAG v20.51.0
)

# nlohmann_json
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)

# zlib
FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG v1.3.1
)

# Fetch and add dependencies
FetchContent_MakeAvailable(SObjectizer)
FetchContent_MakeAvailable(uwebsockets)
FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(zlib)

# --- Dependencies ---
find_package(Threads REQUIRED)

# --- Source Files ---
file(GLOB_RECURSE SOURCES "src/*.cpp")

# --- Executable ---
add_executable(${PROJECT_NAME} ${SOURCES})


# --- Link Libraries ---
target_link_libraries(${PROJECT_NAME}
    SObjectizer::sobjectizer
    Threads::Threads
    uwebsockets::uwebsockets
    json
    zlib::zlib  # Changed this line
)

# --- Include Directories ---
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SObjectizer_SOURCE_DIR}/dev
    ${uwebsockets_SOURCE_DIR}/src
    ${uwebsockets_SOURCE_DIR}/uSockets/src
    ${json_SOURCE_DIR}/include
    src
)

# --- Install Rules (Optional) ---
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# install(DIRECTORY src DESTINATION include)

# --- Enable Testing (Optional) ---
# enable_testing()
# add_test(NAME my_test COMMAND ${PROJECT_NAME} test)

# Copy config file to build folder if exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.json")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/config.json" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()
