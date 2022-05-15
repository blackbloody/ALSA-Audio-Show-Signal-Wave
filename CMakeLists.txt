cmake_minimum_required(VERSION 3.11.4)

project(WP_WAV_SEC)

SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")


##################################################################################################################
##################################################################################################################
##################################################################################################################
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBSNDFILE_PKGCONF sndfile)
endif(PKG_CONFIG_FOUND)

# Include dir
find_path(LIBSNDFILE_INCLUDE_DIR
        NAMES sndfile.h
        PATHS ${LIBSNDFILE_PKGCONF_INCLUDE_DIRS}
        )

# Library
find_library(LIBSNDFILE_LIBRARY
        NAMES sndfile libsndfile-1
        PATHS ${LIBSNDFILE_PKGCONF_LIBRARY_DIRS}
        )

find_package(PackageHandleStandardArgs)
find_package_handle_standard_args(LibSndFile  DEFAULT_MSG  LIBSNDFILE_LIBRARY LIBSNDFILE_INCLUDE_DIR)

if(LIBSNDFILE_FOUND)
    set(LIBSNDFILE_LIBRARIES ${LIBSNDFILE_LIBRARY})
    set(LIBSNDFILE_INCLUDE_DIRS ${LIBSNDFILE_INCLUDE_DIR})
endif(LIBSNDFILE_FOUND)

mark_as_advanced(LIBSNDFILE_LIBRARY LIBSNDFILE_LIBRARIES LIBSNDFILE_INCLUDE_DIR LIBSNDFILE_INCLUDE_DIRS)

include(FindPkgConfig)
pkg_search_module(SndFile REQUIRED sndfile)

include_directories(${LIBSNDFILE_INCLUDE_DIRS})
##################################################################################################################
##################################################################################################################
##################################################################################################################
find_library(LIBSAMPLERATE samplerate)
find_path(LIBSAMPLERATE_INCLUDE_DIR samplerate.h)

include_directories(${LIBSAMPLERATE_INCLUDE_DIR})
##################################################################################################################
##################################################################################################################
##################################################################################################################

find_package(PkgConfig REQUIRED)
pkg_search_module(EIGEN REQUIRED eigen3)

FIND_PACKAGE(ALSA)
IF(ALSA_FOUND)
	SET(WP_WAV_HAVE_ALSA TRUE)
ELSE(ALSA_FOUND)
	MESSAGE("ALSA does not exist..need to install it libasound2-dev")
ENDIF(ALSA_FOUND)

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(GTKMM gtkmm-3.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

FILE (GLOB_RECURSE APP_SOURCE src/app/app_window/*.cpp)

FILE (GLOB_RECURSE READER_SOURCE src/app/reader_wav/*.cpp)
add_library(READER_SOURCE_HEAD SHARED ${READER_SOURCE})
target_include_directories(READER_SOURCE_HEAD PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/app/reader_wav )

FILE (GLOB_RECURSE SFML_WIDGET_SOURCE src/app/app_window/sfml_widget/*.cpp)
add_library(SFML_WIDGET_SOURCE_HEAD SHARED ${SFML_WIDGET_SOURCE})
target_include_directories(SFML_WIDGET_SOURCE_HEAD PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app_window/sfml_widget )

FILE (GLOB_RECURSE OPENGL_DRAW_SOURCE src/app/app_window/sfml_widget/draw/src_draw/*.cpp)
add_library(OPENGL_DRAW_SOURCE_HEAD SHARED ${OPENGL_DRAW_SOURCE})
target_include_directories(OPENGL_DRAW_SOURCE_HEAD PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app_window/sfml_widget/draw )

add_library(LIBROSA_SOURCE_HEAD INTERFACE)
target_include_directories(LIBROSA_SOURCE_HEAD INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/app/librosa )

#################################################################
add_executable(${CMAKE_PROJECT_NAME} src/main.cpp ${APP_SOURCE})
target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_DL_LIBS})
#################################################################

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app_window )

target_link_libraries(${CMAKE_PROJECT_NAME} LIBROSA_SOURCE_HEAD )

target_link_libraries(${CMAKE_PROJECT_NAME} READER_SOURCE_HEAD )

target_link_libraries(${CMAKE_PROJECT_NAME} SFML_WIDGET_SOURCE_HEAD )

target_link_libraries(${CMAKE_PROJECT_NAME} OPENGL_DRAW_SOURCE_HEAD )

################################################################
# --------------------------------------------------------------
# BELOW IS LIBRARY
# --------------------------------------------------------------
################################################################

target_link_libraries(${CMAKE_PROJECT_NAME} ${GTKMM_LIBRARIES})

find_package(SFML 2.5.1 COMPONENTS graphics REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} sfml-graphics )

IF(WP_WAV_HAVE_ALSA)
	target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${ALSA_INCLUDE_DIRS})
	target_link_libraries(${CMAKE_PROJECT_NAME} ${ALSA_LIBRARIES})
ENDIF(WP_WAV_HAVE_ALSA)

message(STATUS "sndfile include dirs path: ${LIBSNDFILE_INCLUDE_DIRS}")
message(STATUS "sndfile libs path: ${LIBSNDFILE_LIBRARIES}")
target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBSNDFILE_LIBRARIES})


message(STATUS "samplerate include dirs path: ${LIBSAMPLERATE_INCLUDE_DIR}")
message(STATUS "samplerate libs path: ${LIBSAMPLERATE}")
target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBSAMPLERATE})

include_directories(${EIGEN_INCLUDE_DIRS})
link_directories(${EIGEN_LIBRARY_DIRS})

find_package(OpenGL)
include_directories( ${OPENGL_INCLUDE_DIRS} )
target_link_libraries(${CMAKE_PROJECT_NAME} ${OPENGL_LIBRARIES} )
