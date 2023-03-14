cmake_minimum_required(VERSION 3.5)

project(teensypolyphony C CXX)

set(SOURCE_FILES
		sampler.cpp
		stringsampler.cpp
		polyphonicsampler.cpp
		loopsampler.cpp)

set(HEADER_FILES
		audiovoicepolyphonic.h
		sampler.h
		stringsampler.h
		polyphonicsampler.h
		polyphonic.h
		TeensyPolyphony.h
		loopsampler.h
		loopsamplerenums.h)

if (NOT DEFINED TEENSY_VERSION)
	include(CMakeLists.linux.cmake)
else()
	include(CMakeLists.teensy.cmake)
endif()