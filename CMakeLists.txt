# ----------------------------------------------------------------------------
#  Root CMake file for IVALib
#
#    From the off-tree build directory, invoke:
#      $ cmake <PATH_TO_IVALIB_ROOT>
#
#
#   - AUG-2012: Initial version <3849058@gmail.com>
#
# ----------------------------------------------------------------------------

#Detecting crosscompiling by presense of CMAKE_TOOLCHAIN_FILE
if(NOT CMAKE_TOOLCHAIN_FILE)
  if(WIN32)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory")
  else()
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation Directory")
  endif()

  if(MSVC)
    set(CMAKE_USE_RELATIVE_PATHS ON CACHE INTERNAL "" FORCE)
  endif()
else(NOT CMAKE_TOOLCHAIN_FILE)
  # any crosscompiling
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory")
endif(NOT CMAKE_TOOLCHAIN_FILE)

#------------------------------------------------------------
#Top Level IVALib Project
#------------------------------------------------------------

#check for correct version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

#Ask user to input build type. Must go before the project command
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
if(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
  set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
endif()

PROJECT(IVALib CXX C)

include(cmake/IVALibUtils.cmake REQUIRED)

#IVALib Build Options
#============================================================
IVA_OPTION(BUILD_SHARED_LIBS        "Builds Shared Link Libraries" ON)
IVA_OPTION(BUILD_DOCS               "Create build rules for IVALib Documentation" ON )
IVA_OPTION(BUILD_WITH_DEBUG_INFO    "Include debug info into debug libs (not MSCV only)" ON )
IVA_OPTION(ENABLE_PROFILING         "Enable profiling in the GCC compiler (Add flags: -g -pg)" OFF  IF CMAKE_COMPILER_IS_GNUCXX )
IVA_OPTION(BUILD_EXAMPLES           "Create build information for IVALib Examples" OFF )
IVA_OPTION(INSTALL_C_EXAMPLES       "Installs the C examples if built and not WIN32" OFF) 


#Find 3rd Party Library and Set build and install layout
#============================================================

#find 3rd party libraries
include(cmake/IVAFindLibs.cmake)


# --- LATEX for pdf documentation ---
if(BUILD_DOCS)
#  include(cmake/IVALibFindLATEX.cmake REQUIRED)
endif(BUILD_DOCS)


# Save libs and executables in the same place
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for applications" )

if(WIN32)
    set(IVALIB_DOC_INSTALL_PATH doc)
elseif(INSTALL_TO_MANGLED_PATHS)
    set(IVALIB_DOC_INSTALL_PATH share/IVALib/doc)
else()
    set(IVALIB_DOC_INSTALL_PATH share/IVALib/doc)
endif()

set(LIBRARY_OUTPUT_PATH         "${IVALib_BINARY_DIR}/lib")
set(3P_LIBRARY_OUTPUT_PATH      "${IVALib_BINARY_DIR}/3rdparty/lib${LIB_SUFFIX}")
set(IVALIB_LIB_INSTALL_PATH     lib${LIB_SUFFIX})
set(IVALIB_3P_LIB_INSTALL_PATH  share/IVALib/3rdparty/${IVALIB_LIB_INSTALL_PATH})
set(IVALIB_INCLUDE_INSTALL_PATH include/IVALib/)

math(EXPR SIZEOF_VOID_P_BITS "8 * ${CMAKE_SIZEOF_VOID_P}")
if(LIB_SUFFIX AND NOT SIZEOF_VOID_P_BITS EQUAL LIB_SUFFIX)
  set(IVALIB_CONFIG_INSTALL_PATH lib${LIB_SUFFIX}/cmake/IVALib)
else()
  set(IVALIB_CONFIG_INSTALL_PATH share/IVALib)
endif()

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${IVALIB_LIB_INSTALL_PATH}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#============================================================
#Process subdirectories
#============================================================
include(cmake/IVALibModules.cmake)

# IVALib include
add_subdirectory(include)

# IVALib modules
add_subdirectory(modules)

# IVALib examples
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()


