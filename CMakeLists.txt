cmake_minimum_required(VERSION 3.14)

if (WIN32)
    project(reMacro LANGUAGES CXX)
elseif(UNIX)
    project(reMacro)
endif()

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)

#======================= INCLUSION OF Qt =======================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(NOT DEFINED ENV{QTDIR})
	MESSAGE(STATUS "QTDIR environment variable not set. Be sure to set the QTDIR
	cmake cache variable to the Qt root install location")
endif()

set(QTDIR $ENV{QTDIR} CACHE STRING "Qt install path")
list(APPEND CMAKE_PREFIX_PATH ${QTDIR})


find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Network)


#=================== INCLUSION OF Project Files ====================#
set(FORMS_DIR "${CMAKE_SOURCE_DIR}/forms")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(SOURCE_DIR_EXT1 "${CMAKE_SOURCE_DIR}/src/Macro")
set(SOURCE_DIR_EXT2 "${CMAKE_SOURCE_DIR}/src/Filters")



include_directories(${FORMS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})
include_directories(${SOURCE_DIR_EXT1})
include_directories(${SOURCE_DIR_EXT2})


file(GLOB_RECURSE SOURCES
    "${FORMS_DIR}/*.ui"
    "${FORMS_DIR}/*.qrc"
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
	"${SOURCE_DIR_EXT1}/*.cpp"
	"${SOURCE_DIR_EXT2}/*.cpp"
	if(WIN32)
		"${SOURCE_DIR}/*.h"
	endif()

)

#=================== SETUP EXECTUABLE ====================#
# Enable debug logging on RELWITHDEBINFO configuration
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>
)

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/res.rc")

# Add the forms directory to the AUTOUIC search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

# Add the executable
if (WIN32) 
    add_executable(reMacro WIN32 ${SOURCES} ${app_icon_resource_windows})
elseif(UNIX)
    add_executable(reMacro ${SOURCES} ${app_icon_resource_windows})
endif()

# Add the target includes for reMacro 
target_include_directories(reMacro PRIVATE ${FORMS_DIR})
target_include_directories(reMacro PRIVATE ${INCLUDE_DIR})
target_include_directories(reMacro PRIVATE ${SOURCE_DIR})
target_precompile_headers(reMacro PUBLIC stdafx.h)

#===================== LINKING LIBRARIES =======================#
target_link_libraries(reMacro Qt6::Widgets)
target_link_libraries(reMacro Qt6::Network)