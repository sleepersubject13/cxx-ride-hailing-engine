cmake_minimum_required(VERSION 4.0)

project(
    RideHailingEngine
    VERSION 1.0
    LANGUAGES CXX
)

# We will need to ensure our compiler supports
# C++20 and above. We will be using std::jthread
# instead of std::thread from the <thread>
# library. The former is only available from C++20
# Additionally, we use other helpful features only
# available with said standard, like std::ranges,
# and projection
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

# Set this option to ON to enable race condition detection using TSan
option(ENABLE_TSAN "Enable Thread Sanitizer for race condition detection" ON)

add_executable(
    ride-hailing-engine
    source/hierachial_mutex.cxx
    source/driver.cxx
    source/rider.cxx
    source/city_zone.cxx
    source/main.cxx
)

target_include_directories(ride-hailing-engine PRIVATE include)
target_link_libraries(ride-hailing-engine PRIVATE Threads::Threads)

# If we have enabled the ThreadSanitizer, we configure it here
if(ENABLE_TSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Building with ThreadSanitizer enabled!")
        target_compile_options(ride-hailing-engine PRIVATE -fsanitize=thread -g -O1)
        target_link_options(ride-hailing-engine PRIVATE -fsanitize=thread)
    else()
        message(WARNING "ThreadSanitizer is only supported on GCC and Clang.")
    endif()
endif()

