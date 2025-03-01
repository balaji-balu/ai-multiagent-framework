cmake_minimum_required(VERSION 3.14)
project(AIFramework)
#enable_testing()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
    sobjectizer
    GIT_REPOSITORY https://github.com/Stiffstream/sobjectizer.git
    GIT_TAG v.5.8.4
)
FetchContent_MakeAvailable(sobjectizer)
add_library(sobjectizer INTERFACE)

# Fetch uWebSockets
FetchContent_Declare(
    uwebsockets
    GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
    GIT_TAG v20.51.0 # Use the latest stable version
)
FetchContent_MakeAvailable(uwebsockets)
add_library(uwebsockets INTERFACE)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2  # Specify the version you want
)
# Make the library available
FetchContent_MakeAvailable(json)
add_library(json INTERFACE)

# Source files
set(SOURCES
    src/main.cpp
    src/agent.cpp
    src/agent_factory.cpp
    src/agent_manager.cpp
    src/logging_service.cpp
    src/learning_agent.cpp
    src/rule_based_agent.cpp
)

# Header files
set(HEADERS
    src/agent_factory.h
    src/agent_manager.h
    src/agent.h
    src/framework.h
    src/logging_service.h
    src/messages.h
    src/websocket_server.h
    src/logging_service.h
)

# Create executable
add_executable(ai_framework ${SOURCES} ${HEADERS})

# Link libraries
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

target_include_directories(ai_framework PRIVATE
    ${sobjectizer_SOURCE_DIR}/dev
    ${uwebsockets_SOURCE_DIR}/src
    ${uwebsockets_SOURCE_DIR}/uSockets/src
)

# Link dependencies
target_link_libraries(ai_framework PRIVATE
    sobjectizer
    Threads::Threads
    OpenSSL::SSL
    uwebsockets
    nlohmann_json::nlohmann_json
)

#add_executable(my_test tests/ActorSystemTests.cpp)
#target_link_libraries(my_test PRIVATE my_library)
#add_test(NAME MyTest COMMAND my_test)