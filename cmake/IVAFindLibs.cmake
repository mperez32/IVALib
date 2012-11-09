#------------------------------------------------------------
#Find all 3rd party Libraries needed to build IVALib
#------------------------------------------------------------
 

#Find most of the packages that will be needed
Find_Package(LAPACK REQUIRED)
if(NOT LAPACK_FOUND)
  MESSAGE(FATAL_ERROR " Lapack library was not found.")
else()
  set(HAVE_LAPACK TRUE)
  MESSAGE("LAPACK FOUND")
endif()

find_package(VTK)
IF(NOT VTK_DIR)
  message(FATAL_ERROR "VTK was not found. Please set VTK_DIR.")
else()
  set(HAVE_VTK TRUE)
  MESSAGE("VTK FOUND")
ENDIF(NOT VTK_DIR)
include(${VTK_USE_FILE})

FIND_PACKAGE( OpenCV)
if(NOT OpenCV_FOUND)
  MESSAGE(FATAL_ERROR " OpenCV library was not found.")
else()
  set(HAVE_OPENCV TRUE)
  MESSAGE("OPENCV FOUND")
endif()
FIND_PACKAGE( Armadillo REQUIRED)
if(NOT ARMADILLO_FOUND)
  MESSAGE(FATAL_ERROR " Armadillo library was not found.")
else()
  set(HAVE_ARMADILLO TRUE)
  #this is flag to link armadillo
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -larmadillo")
  MESSAGE("ARMADILLO FOUND")
endif()

FIND_PACKAGE(PkgConfig)
if(NOT PKGCONFIG_FOUND)
  MESSAGE(FATAL_ERROR " PkgConfig library was not found.")
endif()

set(CORE_INCLUDE_DIRECTORIES "${ARMADILLO_INCLUDE_DIRS} ${OPENCV_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS}" CACHE INTERNAL "Include directories used for most modules")

set(CORE_LIBRARIES_DIRECTORIES "${ARMADILLO_LIBRARY_DIRS} ${OPENCV_LIBRARY_DIRS} ${LAPACK_LIBRARY_DIRS}" CACHE INTERNAL "Libraries used for most modules")

set(CORE_LIBRARIES "${OpenCV_LIBS}" CACHE INTERNAL "Directories of libraries used for most modules")


#Look for all the Libraries used in Plotter module
pkg_check_modules(GTKMM gtkmm-2.4)
if(NOT GTKMM_FOUND)
  MESSAGE(FATAL_ERROR "GTKMM was not found")
endif()
pkg_check_modules(PLOTMM plotmm)
if(NOT PLOTMM_FOUND)
  MESSAGE(FATAL_ERROR "Plotmm was not found")
endif()

SET(VTK_REQUIRED_LIBS  vtkRendering  vtkGraphics vtkHybrid  vtkWidgets vtkImaging vtkIO vtkFiltering vtkCommon vtkVolumeRendering CACHE INTERNAL "VTK Libraries used in IVALIB")

SET(PLOTTER_INCLUDE_DIRS "${GTKMM_INCLUDE_DIRS} ${PLOTMM_INCLUDE_DIRS}"  CACHE INTERNAL  "Include directories used for the Plotter module" )
SET(PLOTTER_LIBRARY_DIRS "${GTKMM_LIBRARY_DIRS} ${PLOTMM_LIBRARY_DIRS}"  CACHE INTERNAL "Library directories used for the Plotter module"  )
SET(PLOTTER_LIBRARIES ${PLOTMM_LIBRARIES} ${GTKMM_LIBRARIES} ${VTK_LIBRARIES}  CACHE INTERNAL "Library used for the Plotter module"  )

#set the linker libaries
#TODO: Need to append all libraries into one variable
#set(IVALIB_LINKER_LIBS "${PLOTTER_LIBRARIES} ${CORE_LIBRARIES}" CACHE INTERNAL "Libraries linked to all libraries")