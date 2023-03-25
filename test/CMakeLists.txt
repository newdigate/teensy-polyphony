cmake_minimum_required(VERSION 3.5)
project(tests)
set(CMAKE_CXX_STANDARD 14)

set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs)
include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_sd_stubs)
include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

find_package(teensy_variable_playback)
include_directories(${teensy_variable_playback_INCLUDE_DIR})

include_directories(../src)

add_executable(tests
#        ../src/polyphonic.cpp
#        ../src/sampleplaymidicontroller.cpp
        output_test.cpp
        test_polyphonic.cpp
        test_polyphonic_sampler.cpp
        test_mysampler.cpp)

target_link_libraries(tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(tests ${teensy_variable_playback_LIBS})
target_link_libraries(tests ${teensy_audio_x86_stubs_LIBS})
target_link_libraries(tests ${teensy_x86_sd_stubs_LIBS})
target_link_libraries(tests ${teensy_x86_stubs_LIBS})