# Require CMAKE 3.20 or higher
cmake_minimum_required(VERSION 3.20)

# Project name
project(pa7)

# Build for C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Turn on any compiler-specific flags
if (WIN32)
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
else()
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
   if ("$ENV{RELEASE}" STREQUAL "ON")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
	endif()
	if ("$ENV{SANITIZE}" STREQUAL "ON")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	endif()
endif()

# Any source files in this directory
set(SOURCE_FILES Main.cpp)

# Where any include files are
include_directories(src)
include_directories(/opt/homebrew/Cellar/cpprestsdk/2.10.19/include )

# Subdirectories to build
add_subdirectory(src)
add_subdirectory(tests)

# Name of executable
add_executable(main ${SOURCE_FILES})

# Link main vs the source library
if(UNIX)
	if(APPLE)
		if(IS_DIRECTORY /opt/homebrew)
			set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl)
		else()
			set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
		endif()
	endif()
	find_library(cpprestsdk-lib cpprest)
	find_package(Boost REQUIRED COMPONENTS random system thread filesystem chrono atomic date_time regex)
	find_package(OpenSSL 1.0.0 REQUIRED)
	target_link_libraries(main src
		${cpprestsdk-lib}
		${Boost_LIBRARIES}
		OpenSSL::SSL
	)
else()
	find_package(cpprestsdk REQUIRED)
	target_link_libraries(main src cpprestsdk::cpprest)
endif()
