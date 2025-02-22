cmake_minimum_required(VERSION 3.10)
project(polyphony_demo)

set(CMAKE_CXX_STANDARD 14)
teensy_include_directories(../../src)
teensy_add_executable(polyphony_demo polyphony_demo.ino)
teensy_target_link_libraries(polyphony_demo  MIDI teensy_variable_playback Audio  SD SdFat  SPI SerialFlash  Wire  cores)
target_link_libraries(polyphony_demo.elf m stdc++)