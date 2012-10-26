# ----------------------------------------------------------------------------
#  CMake file for C examples. See root CMakeLists.txt
#
# ----------------------------------------------------------------------------

SET(IVALIB_CPP_EXAMPLES_REQUIRED_DEPS ivalib_plotter ivalib_activesurfaces ivalib_threed)


iva_check_dependencies(${IVALIB_CPP_EXAMPLES_REQUIRED_DEPS})


if(BUILD_EXAMPLES AND IVALIB_DEPENDENCIES_FOUND)
  project(cpp_samples)
  message("Dependencies found")
endif()
 
iva_include_modules(${IVALIB_CPP_SAMPLES_REQUIRED_DEPS} ivalib_plotter ivalib_activesurfaces ivalib_utils ivalib_threed)
  
iva_include_directories(${PLOTTER_INCLUDE_DIRS}  "${IVALib_SOURCE_DIR}/include/IVALib/")

# ---------------------------------------------
#      Define executable targets
# ---------------------------------------------
MACRO(IVALIB_DEFINE_CPP_EXAMPLE name srcs)
  set(the_target "example_${name}")
  add_executable(${the_target} ${srcs})

  target_link_libraries(${the_target} ${IVALIB_CPP_EXAMPLES_REQUIRED_DEPS} ${CORE_LIBRARIES} ${PLOTTER_LIBRARIES} )
  
  
  set_target_properties(${the_target} PROPERTIES
    OUTPUT_NAME "${name}"
    PROJECT_LABEL "(EXAMPLE) ${name}")
  
  
  
  if(WIN32)
    if (MSVC AND NOT BUILD_SHARED_LIBS)
      set_target_properties(${the_target} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:atlthunk.lib /NODEFAULTLIB:atlsd.lib /DEBUG")
    endif()
    install(TARGETS ${the_target}
      RUNTIME DESTINATION "examples/cpp" COMPONENT main)
  endif()
ENDMACRO()


file(GLOB cpp_examples RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(example_filename ${cpp_examples})
  get_filename_component(example ${example_filename} NAME_WE)
  IVALIB_DEFINE_CPP_EXAMPLE(${example} ${example_filename})
endforeach()

if (INSTALL_C_EXAMPLES AND NOT WIN32)
  file(GLOB C_SAMPLES *.c *.cpp *.jpg *.png *.data makefile.*  *.dsp *.cmd )
  install(FILES ${C_SAMPLES}
          DESTINATION share/OpenCV/samples/cpp
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
endif()
