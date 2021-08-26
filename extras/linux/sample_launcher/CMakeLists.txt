cmake_minimum_required(VERSION 3.10)
project(sample_launcher)
set(CMAKE_CXX_STANDARD 14)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs)
include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs_soundio)
include_directories(${teensy_audio_x86_stubs_soundio_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs)
include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_sd_stubs)
include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

find_package(teensy_variable_playback)
include_directories(${teensy_variable_playback_INCLUDE_DIR})

find_package(teensy_x86_midi_stubs)
include_directories(${teensy_x86_midi_stubs_INCLUDE_DIR})

find_package(teensy_x86_midi_stubs_rtmidi)
include_directories(${teensy_x86_midi_stubs_rtmidi_INCLUDE_DIR})

find_path(RTMIDI_INCLUDE_DIR NAMES RtMidi.h)
include_directories(${RTMIDI_INCLUDE_DIR})

find_package(teensy_st7735_linux_stubs)
include_directories(${teensy_st7735_linux_stubs_INCLUDE_DIR})

find_package(teensy_st7735_linux_opengl)
include_directories(${teensy_st7735_linux_extras_opengl_INCLUDE_DIR})

include_directories(../../../src)

add_executable(sample_launcher sample_launcher.cpp)

target_link_libraries(sample_launcher ${teensy_audio_x86_stubs_soundio_LIBS})
target_link_libraries(sample_launcher -L/usr/local/lib/ rtmidi)
target_link_libraries(sample_launcher ${teensy_variable_playback_LIBS})
target_link_libraries(sample_launcher ${teensy_audio_x86_stubs_LIBS})
target_link_libraries(sample_launcher ${teensy_x86_sd_stubs_LIBS})
target_link_libraries(sample_launcher ${teensy_x86_stubs_LIBS})
target_link_libraries(sample_launcher ${teensy_x86_midi_stubs_LIBS})
target_link_libraries(sample_launcher ${teensy_x86_midi_stubs_rtmidi_LIBS})
target_link_libraries(sample_launcher ${teensy_st7735_linux_stubs_LIBS})
target_link_libraries(sample_launcher ${teensy_st7735_linux_extras_opengl_LIBS})

include(opengl.cmake)
target_link_libraries(sample_launcher ${LIBS})

target_link_libraries(sample_launcher "-L/usr/local/lib -lsoundio")
if  (APPLE)
        target_link_libraries(sample_launcher
                "-framework CoreServices"
                "-framework CoreAudio"
                "-framework Foundation"
                "-framework AudioUnit")
endif()
