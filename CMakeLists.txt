CMAKE_MINIMUM_REQUIRED(VERSION "3.12")

set(CMAKE_CXX_STANDARD 14)
#this is set manually for the lab computers this should not be required for other systems with proper installation
#		set(CMAKE_TOOLCHAIN_FILE "K:\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")
project( DisplayImage )

#set(OpenCV_DIR "K:\\vcpkg\\installed\\x64-windows")

find_package(OpenCV REQUIRED)#find_package (OpenCV CONFIG REQUIRED)
include_directories (${OpenCV_INCLUDE_DIRS})
add_executable( DisplayImage ocvTest.cpp   )

target_link_libraries(DisplayImage
  ${OpenCV_LIBS}
)