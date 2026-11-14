cmake_minimum_required(VERSION 4.0)

project(
    RideHailingEngine
    VERSION 1.0
    LANGUAGES CXX
)

# You will need to ensure your compiler supports
# C++20 and above. We will be using std::jthread
# instead of std::thread from the <thread>
# library. The former is only available from C++20
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(
    ride-hailing-engine
    source/main.cxx
)

target_include_directories(ride-hailing-engine PRIVATE include)
target_link_libraries(ride-hailing-engine PRIVATE Threads::Threads)

