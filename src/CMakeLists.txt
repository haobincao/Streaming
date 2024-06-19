# If you create new headers/cpp files, add them to these list!
set(HEADER_FILES
	SrcMain.h
)

set(SOURCE_FILES
	SrcMain.cpp
)

if (APPLE)
	include_directories(/usr/local/include)
endif()

# Don't change this
add_library(src ${SOURCE_FILES} ${HEADER_FILES})

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
	target_link_libraries(src PRIVATE
		${cpprestsdk-lib}
		${Boost_LIBRARIES}
		OpenSSL::SSL
	)
else()
	find_package(cpprestsdk REQUIRED)
	target_link_libraries(src PRIVATE cpprestsdk::cpprest)
endif()
