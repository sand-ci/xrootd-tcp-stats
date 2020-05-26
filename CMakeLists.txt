cmake_minimum_required( VERSION 2.8 )
project( xrootd-tcp-stats )

set( CMAKE_MODULE_PATH /usr/local/share/xrootd/cmake /usr/share/xrootd/cmake )

find_package( XRootD REQUIRED CONFIG )

if(NOT XROOTD_PLUGIN_VERSION)
  find_program(XROOTD_CONFIG_EXECUTABLE xrootd-config)
  exec_program(${XROOTD_CONFIG_EXECUTABLE} ARGS "--plugin-version" OUTPUT_VARIABLE XROOTD_PLUGIN_VERSION RETURN_VALUE RETVAR)
  set(XROOTD_PLUGIN_VERSION ${XROOTD_PLUGIN_VERSION} CACHE INTERNAL "")
endif()

# Check out picojson submodule
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/picojson/picojson.h")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()


macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)
use_cxx11()

if( CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror" )
endif()
SET( CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
SET( CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-undefined")

include_directories(${XROOTD_INCLUDE_DIRS} vendor/picojson)

add_library(XrdTCPStats-${XROOTD_PLUGIN_VERSION} SHARED src/tcpstats.cpp)
target_link_libraries(XrdTCPStats-${XROOTD_PLUGIN_VERSION} -ldl -lpthread ${XROOTD_LIBRARIES})
#set_target_properties(XrdTCPStats-${XROOTD_PLUGIN_VERSION} PROPERTIES OUTPUT_NAME XrdTCPStats-${XROOTD_PLUGIN_VERSION} SUFFIX ".so" LINK_FLAGS "-Wl,--version-script=${CMAKE_SOURCE_DIR}/configs/export-lib-symbols")
set_target_properties(XrdTCPStats-${XROOTD_PLUGIN_VERSION} PROPERTIES OUTPUT_NAME XrdTCPStats-${XROOTD_PLUGIN_VERSION} SUFFIX ".so")


SET(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Install path for libraries")

install(
  TARGETS XrdTCPStats-${XROOTD_PLUGIN_VERSION}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR})