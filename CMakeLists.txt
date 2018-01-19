cmake_minimum_required(VERSION 3.5.0)

project(rco-dump)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)

add_executable(rco-dump
	${CMAKE_SOURCE_DIR}/src/rco-dump.cpp
	${CMAKE_SOURCE_DIR}/src/rco.cpp
	${CMAKE_SOURCE_DIR}/src/layout.cpp
	${CMAKE_SOURCE_DIR}/src/util.cpp
	${CMAKE_SOURCE_DIR}/include/rco.h
	${CMAKE_SOURCE_DIR}/include/layout.h
	${CMAKE_SOURCE_DIR}/include/util.h
	${CMAKE_SOURCE_DIR}/include/platform.h)

include_directories(${CMAKE_SOURCE_DIR}/include)

link_directories(${CMAKE_SOURCE_DIR}/lib)

if (WIN32)

	find_library(ZLIB_LIBRARY zlibstatic.lib HINTS ${CMAKE_SOURCE_DIR}/libs/zlib/libs/x86/static/Debug)
	include_directories(${CMAKE_SOURCE_DIR}/libs/zlib/include)

	target_link_libraries(rco-dump
		${ZLIB_LIBRARY})

	# Keep this until siblings no longer recurse
	set_target_properties(rco-dump PROPERTIES LINK_FLAGS_DEBUG "/STACK:\"4194304\"\",8096\"")

endif (WIN32)

if (UNIX)
	find_package(ZLIB REQUIRED)
	if (ZLIB_FOUND)
		include_directories(${ZLIB_INCLUDE_DIRS})
		target_link_libraries(rco-dump ${ZLIB_LIBRARIES})

	endif (ZLIB_FOUND)
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -std=c++11 ${GCC_COVERAGE_COMPILE_FLAGS}" )
endif(UNIX)
