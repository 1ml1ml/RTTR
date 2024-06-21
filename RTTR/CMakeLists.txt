get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)

project(${CURRENT_DIR_NAME})

file(GLOB_RECURSE src src/*.h src/*.cpp src/*.hpp src/*.c)

include_directories(
	"src" 
	"src/RTTR")

add_executable(${PROJECT_NAME} ${src})
