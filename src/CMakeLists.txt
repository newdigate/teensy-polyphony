cmake_minimum_required(VERSION 3.5)

project(teensypolyphony C CXX)

set(SOURCE_FILES
		sampler.cpp
		stringsampler.cpp
		polyphonicsampler.cpp
		sampleplaymidicontroller.cpp
		loopsampler.cpp)

set(HEADER_FILES
		sampler.h
		stringsampler.h
		polyphonicsampler.h
		TeensyPolyphony.h
		sampleplaymidicontroller.h
		sampleplaymidicontrollerenums.h
		abstractdisplay.h
		serialdisplay.h
		loopsampler.h
		loopsamplerenums.h)

if (NOT DEFINED TEENSY_VERSION)
	include(CMakeLists.linux.cmake)
else()
	include(CMakeLists.teensy.cmake)
endif()