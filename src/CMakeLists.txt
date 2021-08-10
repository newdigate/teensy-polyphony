cmake_minimum_required(VERSION 3.5)

project(teensypolyphony C CXX)

set(SOURCE_FILES
		sampler.cpp
		stringsampler.cpp
		polyphonicsampler.cpp)

set(HEADER_FILES
		sampler.h
		stringsampler.h
		polyphonicsampler.h
		TeensyPolyphony.h)

if (NOT DEFINED TEENSY_VERSION)
	include(CMakeLists.linux.cmake)
else()
	include(CMakeLists.teensy.cmake)
endif()