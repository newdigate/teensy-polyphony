cmake_minimum_required(VERSION 3.10)
project(launch_ctrl)

set(CMAKE_CXX_STANDARD 14)
teensy_include_directories(../../src)
teensy_add_executable(launch_ctrl launch_ctrl.ino)
teensy_target_link_libraries(launch_ctrl  USBHost_t36 MIDI TeensyVariablePlayback Audio  SD SdFat  SPI SerialFlash  Wire  cores)    
target_link_libraries(launch_ctrl.elf m stdc++)