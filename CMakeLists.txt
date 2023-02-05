cmake_minimum_required(VERSION 3.10.0)
project(Cerveurus C)

# build cJSON dependency
add_subdirectory(ext/cJSON)

set(CMAKE_C_STANDARD 99)

include(CTest)

option(RELEASE_MODE OFF)

if (${RELEASE_MODE})
	set(CMAKE_C_FLAGS "-O2 -Wall -Wextra")
else()
	set(CMAKE_C_FLAGS "-O0 -g -Wall -Wextra -Wpedantic -fsanitize=address")
endif()

add_subdirectory(src)

enable_testing()
#add_subdirectory(test)

# copy static/templates/sql folders to the build directory
file(COPY ${Cerveurus_SOURCE_DIR}/static DESTINATION ${Cerveurus_SOURCE_DIR}/build/src)
file(COPY ${Cerveurus_SOURCE_DIR}/templates DESTINATION ${Cerveurus_SOURCE_DIR}/build/src)
file(COPY ${Cerveurus_SOURCE_DIR}/sql DESTINATION ${Cerveurus_SOURCE_DIR}/build/src)
