cmake_minimum_required(VERSION 3.10)
project(loop_ctrl)

set(CMAKE_CXX_STANDARD 14)
import_arduino_library_git(ST7735_t3 https://github.com/PaulStoffregen/ST7735_t3 master src)

teensy_include_directories(../../src)
teensy_add_executable(loop_ctrl loop_ctrl.ino)
teensy_target_link_libraries(loop_ctrl ST7735_t3 USBHost_t36 MIDI teensy_variable_playback Audio  SD SdFat  SPI SerialFlash  Wire  cores)
target_link_libraries(loop_ctrl.elf m stdc++)