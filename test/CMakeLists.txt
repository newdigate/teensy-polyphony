cmake_minimum_required(VERSION 3.5)
project(tests)
set(CMAKE_CXX_STANDARD 17)

set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

include_directories(../src)

add_executable(tests
        output_test.cpp
        test_polyphonic.cpp
        test_polyphonic_sampler.cpp
        test_mysampler.cpp)

target_link_libraries(tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(tests teensy_variable_playback)
target_link_libraries(tests teensy_audio_x86_stubs)
target_link_libraries(tests teensy_x86_sd_stubs)
target_link_libraries(tests teensy_x86_stubs)