# Author: e1027470@student.tuwien.ac.at

cmake_minimum_required(VERSION 3.1)
project(vis2)

#### detect operating system ####
# user arch only if needed
#if(WIN32)
#  set(SYSTEM "Windows")
#  set(SYS "${SYSTEM}${ARCH}")
#elseif(UNIX)
#  set(SYSTEM "Unix")
#  set(SYS "${SYSTEM}${ARCH}")
#endif()
#message("System detected: ${SYS}")

#### cmake config for paths #####
#configure_file ( 
#  "${PROJECT_SOURCE_DIR}/inc/cmake_conf.h.in"
#  "${PROJECT_BINARY_DIR}/inc/cmake_conf.h")

#### find all sourcecode files
include_directories(inc)
include_directories(ext/glm)

#include_directories(${PROJECT_BINARY_DIR}/inc)

#### use glew ####
#find_package(GLEW)
set(GLEW_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/ext/glew-1.10.0/include/")
set(GLEW_LIBRARIES "${PROJECT_SOURCE_DIR}/ext/glew-1.10.0/lib/Release/x64/glew32.lib" "${PROJECT_SOURCE_DIR}/ext/glew-1.10.0/lib/Release/x64/glew32s.lib")
include_directories(${GLEW_INCLUDE_DIRS})
#link_directories(${PCL_LIBRARY_DIRS})

#### anttweakbar ####
add_library(anttweakbar STATIC IMPORTED)
if(WIN32)
	set_target_properties(anttweakbar PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ext/AntTweakBar/lib/AntTweakBar64.lib)
else()
	set_target_properties(anttweakbar PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ext/AntTweakBar/lib/libAntTweakBar.a)
endif()
include_directories(ext/AntTweakBar/include)

#### use glfw ####
if(WIN32)
	add_subdirectory(ext/glfw)
	include_directories(ext/glfw/include)
else()
	find_package(PkgConfig REQUIRED)
	pkg_search_module(GLFW REQUIRED glfw3)
	include_directories(${GLFW_INCLUDE_DIRS})
endif()

#### use pcl ####
set(PCL_DIR ${CMAKE_SOURCE_DIR}/ext/PCL/cmake)
set(FLANN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/ext/PCL/3rdparty/flann/FLANN/include)
set(FLANN_LIBRARY ${CMAKE_SOURCE_DIR}/ext/PCL/3rdparty/flann/FLANN/lib/flann.lib)

find_package(PCL REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})
include_directories(${EIGEN_INCLUDE_DIRS})
include_directories(${BOOST_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
link_directories(${BOOST_LIBRARY_DIRS})

FILE (GLOB HEADERS inc/*.h)
FILE (GLOB SOURCE src/*.cpp)

ADD_EXECUTABLE(vis2 ${HEADERS} ${SOURCE})

#if(WIN32)
    target_link_libraries(vis2 
	  ${GLEW_LIBRARIES} glfw ${GLFW_LIBRARIES} anttweakbar
	  ${PCL_COMMON_LIBRARIES} ${PCL_KDTREE_LIBRARIES} ${PCL_IO_LIBRARIES})
	message("${PCL_LIBRARY_DIRS} ${PCL_COMMON_LIBRARIES} ${PCL_KDTREE_LIBRARIES} ${PCL_IO_LIBRARIES}")
	
#elseif(UNIX)
#endif()

add_custom_command(TARGET vis2 POST_BUILD        # Adds a post-build event to MyTest
	COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
		"${PROJECT_SOURCE_DIR}/ext/glew-1.10.0/bin/Release/x64/glew32.dll"      # <--this is in-file
		$<TARGET_FILE_DIR:vis2>)
		
add_custom_command(TARGET vis2 POST_BUILD        # Adds a post-build event to MyTest
	COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
		"${PROJECT_SOURCE_DIR}/ext/AntTweakBar/lib/AntTweakBar64.dll"      # <--this is in-file
		$<TARGET_FILE_DIR:vis2>)

add_custom_command(TARGET vis2 POST_BUILD        # Adds a post-build event to MyTest
	COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
		"${PROJECT_SOURCE_DIR}/ext/PCL/bin"      # <--this is in-file
		$<TARGET_FILE_DIR:vis2>)

		

