#------------------------------------------------------------
#Macros and functions to define differet modules in 
#IVALibs
#
#Local variables set for each module:
# 
#    name : this is the module name e.g. activecontour
#    the_module : full name of the module e.g. ivalib_activecontour

#Global variables:
#
#    IVALIB_MODULE_${the_module}_LOCATION
#    IVALIB_MODULE_${the_module}_DESCRIPTION
#    IVALIB_MODULE_${the_module}_HEADERS
#    IVALIB_MODULE_${the_module}_SOURCE
#    IVALIB_MODULE_${the_module}_DEPS -final flatten set of module dependencies
#    IVALIB_MODULE_${the_module}_REQ_DEPS
#    IVALIB_MODULE_${the_module}_OPT_DEPS
#    IVALIB_MODULE_${the_module}_3P_INCLUDE-Inlcude directories of the other modules
#    HAVE_{the_module}
    

#------------------------------------------------------------

# clean flags for modules enabled on previous cmake run
# this is necessary to correctly handle modules removal
foreach(mod ${IVALIB_MODULES_BUILD} ${IVALIB_MODULES_DISABLED_USER} ${IVALIB_MODULES_DISABLED_AUTO} ${IVALIB_MODULES_DISABLED_FORCE})
  if(HAVE_${mod})
    unset(HAVE_${mod} CACHE)
  endif()
  unset(IVALIB_MODULE_${mod}_REQ_DEPS CACHE)
  unset(IVALIB_MODULE_${mod}_OPT_DEPS CACHE)
endforeach()

# clean modules info which needs to be recalculated
set(IVALIB_MODULES_PUBLIC         "" CACHE INTERNAL "List of IVALib modules marked for export")
set(IVALIB_MODULES_BUILD          "" CACHE INTERNAL "List of IVALib modules included into the build")
set(IVALIB_MODULES_DISABLED_USER  "" CACHE INTERNAL "List of IVALib modules explicitly disabled by user")
set(IVALIB_MODULES_DISABLED_AUTO  "" CACHE INTERNAL "List of IVALib modules implicitly disabled due to dependencies")
set(IVALIB_MODULES_DISABLED_FORCE "" CACHE INTERNAL "List of IVALib modules which can not be build in current configuration")



#Macro used to add dependencies
#Usage:
#   iva_add_dependencies(the_module [REQUIRED] [<list of required dependencies>] [OPTIONAL <list of optional dependencies>])
macro(iva_add_dependencies full_modname)
  #don't clean any list here so macro can be used multiple times
  foreach(d "REQUIRED" ${ARGN})
    if(d STREQUAL "REQUIRED")
      set(__depsvar IVA_MODULE_${full_modname}_REQ_DEPS)
    elseif(d STREQUAL "OPTIONAL")
      set(__depsvar IVA_MODULE_${full_modname}_OPT_DEPS)
    else()
      list(APPEND ${__depsvar} "${d}")
    endif()
  endforeach()
  unset(__depsvar)

  iva_list_unique(IVALIB_MODULE_${full_modname}_REQ_DEPS)
  iva_list_unique(IVALIB_MODULE_${full_modname}_REQ_DEPS)

  set(IVALIB_MODULE_${full_modname}_REQ_DEPS ${IVALIB_MODULE_${full_modname}_REQ_DEPS} CACHE INTERNAL "Required dependencis of 
${full_modname} module") 
  set(IVALIB_MODULE_${full_modname}_OPT_DEPS ${IVALIB_MODULE_${full_modname}_OPT_DEPS} CACHE INTERNAL "Optional dependencis of 
${full_modname} module") 
endmacro()

# Declares a new IVALib Module in current direcrtory
# Usage:
#   iva_add_module(<name> [INTERNAL|BINDING] [REQUIRED] [<list of dependencies>] [OPTIONAL <list of dependencies>)
macro(iva_add_module _name)
  set(realname "${_name}")
  string(TOLOWER "${_name}" name)
  string(REGEX REPLACE "^ivalib_" "" ${name} "${name}")
  set(the_module ivalib_${name})

  #the first pass collects info the second past builds
  if(IVALIB_INITIAL_PASS)
    #check to see 
    if(";${IVALIB_MODULES_BUILD};${IVALIB_MODULES_DISABLED_USER};" MATCHES ";${the_module};")
      message(FATAL_ERROR "Redefinition of the ${the_module} module.
  at:                    ${CMAKE_CURRENT_SOURCE_DIR}
  previously defined at: ${IVALIB_MODULE_${the_module}_LOCATION}
")
    endif()
    
    if(NOT DEFINED the_description)
      set(the_description "The ${name} IVALib module")
    endif()

    if(NOT DEFINED BUILD_${the_module}_INIT)
      set(BUILD_${the_module}_INIT ON)
    endif()
    
    # create option to enable/disable this module
    option(BUILD_${the_module} "Include ${the_module} module into the IVALib build" ${BUILD_${the_module}_INIT})
    
    # remember the module details
    set(IVALIB_MODULE_${the_module}_DESCRIPTION "${the_description}" CACHE INTERNAL "Brief description of ${the_module} module")
    set(IVALIB_MODULE_${the_module}_LOCATION    "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Location of ${the_module} module sources")

        # parse list of dependencies
    if("${ARGV1}" STREQUAL "INTERNAL" OR "${ARGV1}" STREQUAL "BINDINGS")
      set(IVALIB_MODULE_${the_module}_CLASS "${ARGV1}" CACHE INTERNAL "The cathegory of the module")
      set(__iva_argn__ ${ARGN})
      list(REMOVE_AT __iva_argn__ 0)
      iva_add_dependencies(${the_module} ${__iva_argn__})
      unset(__iva_argn__)
    else()
      set(IVALIB_MODULE_${the_module}_CLASS "PUBLIC" CACHE INTERNAL "The cathegory of the module")
      iva_add_dependencies(${the_module} ${ARGN})
      if(BUILD_${the_module})
        set(IVALIB_MODULES_PUBLIC ${IVALIB_MODULES_PUBLIC} "${the_module}" CACHE INTERNAL "List of IVALib modules marked for export")
      endif()
    endif()

    if(BUILD_${the_module})
      set(IVALIB_MODULES_BUILD ${IVALIB_MODULES_BUILD} "${the_module}" CACHE INTERNAL "List of IVALib modules included into the build")
    else()
      set(IVALIB_MODULES_DISABLED_USER ${IVALIB_MODULES_DISABLED_USER} "${the_module}" CACHE INTERNAL "List of IVALib modules explicitly disabled by user")
    endif()

    # TODO: add submodules if any

    # stop processing of current file
    return()
  else(IVALIB_INITIAL_PASS)
    if(NOT BUILD_${the_module})
      return() # extra protection from redefinition
    endif()
    project(${the_module})
  endif(IVALIB_INITIAL_PASS)
endmacro()

# excludes module from current configuration
macro(iva_module_disable module)
  set(__modname ${module})
  if(NOT __modname MATCHES "^ivalib_")
    set(__modname ivalib_${module})
  endif()
  list(APPEND OPENCV_MODULES_DISABLED_FORCE "${__modname}")
  set(HAVE_${__modname} OFF CACHE INTERNAL "Module ${__modname} can not be built in current configuration")
  set(IVALIB_MODULE_${__modname}_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Location of ${__modname} module sources")
  set(IVALIB_MODULES_DISABLED_FORCE "${OPENCV_MODULES_DISABLED_FORCE}" CACHE INTERNAL "List of OpenCV modules which can not be build in current configuration")
  unset(__modname)
  return() # leave the current folder
endmacro()

# excludes module from current configuration
macro(iva_module_disable module)
  set(__modname ${module})
  if(NOT __modname MATCHES "^ivalib_")
    set(__modname ivalib_${module})
  endif()
  list(APPEND IVALIB_MODULES_DISABLED_FORCE "${__modname}")
  set(HAVE_${__modname} OFF CACHE INTERNAL "Module ${__modname} can not be built in current configuration")
  set(IVALIB_MODULE_${__modname}_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Location of ${__modname} module sources")
  set(IVALIB_MODULES_DISABLED_FORCE "${IVALIB_MODULES_DISABLED_FORCE}" CACHE INTERNAL "List of IVALib modules which can not be build in current configuration")
  unset(__modname)
  return() # leave the current folder
endmacro()

# Internal macro; partly disables IVALib module
macro(__iva_module_turn_off the_module)
  list(APPEND IVALIB_MODULES_DISABLED_AUTO "${the_module}")
  list(REMOVE_ITEM IVALIB_MODULES_BUILD "${the_module}")
  list(REMOVE_ITEM IVALIB_MODULES_PUBLIC "${the_module}")
  set(HAVE_${the_module} OFF CACHE INTERNAL "Module ${the_module} can not be built in current configuration")
endmacro()

# Internal macro for dependencies tracking
macro(__iva_flatten_module_required_dependencies the_module)
  set(__flattened_deps "")
  set(__resolved_deps "")
  set(__req_depends ${IVALIB_MODULE_${the_module}_REQ_DEPS})

  while(__req_depends)
    iva_list_pop_front(__req_depends __dep)
    if(__dep STREQUAL the_module)
      __iva_module_turn_off(${the_module}) # TODO: think how to deal with cyclic dependency
      break()
    elseif(";${IVALIB_MODULES_DISABLED_USER};${IVALIB_MODULES_DISABLED_AUTO};" MATCHES ";${__dep};")
      __iva_module_turn_off(${the_module}) # depends on disabled module
      break()
    elseif(";${IVALIB_MODULES_BUILD};" MATCHES ";${__dep};")
      if(";${__resolved_deps};" MATCHES ";${__dep};")
        list(APPEND __flattened_deps "${__dep}") # all dependencies of this module are already resolved
      else()
        # put all required subdependencies before this dependency and mark it as resolved
        list(APPEND __resolved_deps "${__dep}")
        list(INSERT __req_depends 0 ${IVALIB_MODULE_${__dep}_REQ_DEPS} ${__dep})
      endif()
    elseif(__dep MATCHES "^ivalib_")
      __ocv_module_turn_off(${the_module}) # depends on missing module
      message(WARNING "Unknown \"${__dep}\" module is listened in the dependencies of \"${the_module}\" module")
      break()
    else()
      # skip non-modules
    endif()
  endwhile()
  if(__flattened_deps)
    list(REMOVE_DUPLICATES __flattened_deps)
    set(IVALIB_MODULE_${the_module}_DEPS ${__flattened_deps})
  else()
    set(IVALIB_MODULE_${the_module}_DEPS "")
  endif()

  iva_clear_vars(__resolved_deps __flattened_deps __req_depends __dep)
endmacro()

# Internal macro for dependencies tracking
macro(__iva_flatten_module_optional_dependencies the_module)
  set(__flattened_deps "")
  set(__resolved_deps "")
  set(__opt_depends ${IVALIB_MODULE_${the_module}_REQ_DEPS} ${IVALIB_MODULE_${the_module}_OPT_DEPS})

  while(__opt_depends)
    iva_list_pop_front(__opt_depends __dep)
    if(__dep STREQUAL the_module)
      __iva_module_turn_off(${the_module}) # TODO: think how to deal with cyclic dependency
      break()
    elseif(";${IVALIB_MODULES_BUILD};" MATCHES ";${__dep};")
      if(";${__resolved_deps};" MATCHES ";${__dep};")
        list(APPEND __flattened_deps "${__dep}") # all dependencies of this module are already resolved
      else()
        # put all subdependencies before this dependency and mark it as resolved
        list(APPEND __resolved_deps "${__dep}")
        list(INSERT __opt_depends 0 ${IVALIB_MODULE_${__dep}_REQ_DEPS} ${IVALIB_MODULE_${__dep}_OPT_DEPS} ${__dep})
      endif()
    else()
      # skip non-modules or missing modules
    endif()
  endwhile()

  if(__flattened_deps)
    list(REMOVE_DUPLICATES __flattened_deps)
    set(IVALIB_MODULE_${the_module}_DEPS ${__flattened_deps})
  else()
    set(IVALIB_MODULE_${the_module}_DEPS "")
  endif()

  iva_clear_vars(__resolved_deps __flattened_deps __opt_depends __dep)
endmacro()


macro(__iva_flatten_module_dependencies)
  foreach(m ${IVALIB_MODULES_DISABLED_USER})
    set(HAVE_${m} OFF CACHE INTERNAL "Module ${m} will not be built in current configuration")
  endforeach()
  foreach(m ${IVALIB_MODULES_BUILD})
    set(HAVE_${m} ON CACHE INTERNAL "Module ${m} will be built in current configuration")
    __iva_flatten_module_required_dependencies(${m})
  endforeach()

  foreach(m ${IVALIB_MODULES_BUILD})
    __iva_flatten_module_optional_dependencies(${m})

    # save dependencies from other modules
    set(IVALIB_MODULE_${m}_DEPS ${IVALIB_MODULE_${m}_DEPS} CACHE INTERNAL "Flattened dependencies of ${m} module")
    # save extra dependencies
    set(IVALIB_MODULE_${m}_DEPS_EXT ${IVALIB_MODULE_${m}_REQ_DEPS} ${IVALIB_MODULE_${m}_OPT_DEPS})
    if(IVALIB_MODULE_${m}_DEPS_EXT AND IVALIB_MODULE_${m}_DEPS)
      list(REMOVE_ITEM IVALIB_MODULE_${m}_DEPS_EXT ${IVALIB_MODULE_${m}_DEPS})
    endif()
    iva_list_filterout(IVALIB_MODULE_${m}_DEPS_EXT "^ivalib_[^ ]+$")
    set(IVALIB_MODULE_${m}_DEPS_EXT ${IVALIB_MODULE_${m}_DEPS_EXT} CACHE INTERNAL "Extra dependencies of ${m} module")
  endforeach()

  # order modules by dependencies
  set(IVALIB_MODULES_BUILD_ "")
  foreach(m ${IVALIB_MODULES_BUILD})
    list(APPEND IVALIB_MODULES_BUILD_ ${IVALIB_MODULE_${m}_DEPS} ${m})
  endforeach()
  iva_list_unique(IVALIB_MODULES_BUILD_)

  set(IVALIB_MODULES_BUILD         ${IVALIB_MODULES_BUILD_}         CACHE INTERNAL "List of IVALib modules included into the build")
  set(IVALIB_MODULES_DISABLED_AUTO ${IVALIB_MODULES_DISABLED_AUTO} CACHE INTERNAL "List of IVALib modules implicitly disabled due to dependencies")
endmacro()


# collect modules from specified directories
# NB: must be called only once!
macro(iva_glob_modules)
  if(DEFINED IVALIB_INITIAL_PASS)
    message(FATAL_ERROR "IVALib has already loaded its modules. Calling iva_glob_modules second time is not allowed.")
  endif()
  set(__directories_observed "")

  # collect modules
  set(IVALIB_INITIAL_PASS ON)
  foreach(__path ${ARGN})
    iva_get_real_path(__path "${__path}")

    list(FIND __directories_observed "${__path}" __pathIdx)
    if(__pathIdx GREATER -1)
      message(FATAL_ERROR "The directory ${__path} is observed for OpenCV modules second time.")
    endif()
    list(APPEND __directories_observed "${__path}")

    file(GLOB __ivamodules RELATIVE "${__path}" "${__path}/*")
    if(__ivamodules)
      list(SORT __ivamodules)
      foreach(mod ${__ivamodules})
        iva_get_real_path(__modpath "${__path}/${mod}")
        if(EXISTS "${__modpath}/CMakeLists.txt")

          list(FIND __directories_observed "${__modpath}" __pathIdx)
          if(__pathIdx GREATER -1)
            message(FATAL_ERROR "The module from ${__modpath} is already loaded.")
          endif()
          list(APPEND __directories_observed "${__modpath}")

          #file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${mod}/.${mod}")
          #file(COPY "${__modpath}/CMakeLists.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${mod}/.${mod}")
          add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${mod}/")
          if("${IVALIB_MODULE_ivalib_${mod}_LOCATION}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}/${mod}/.${mod}")
            set(IVALIB_MODULE_ivalib_${mod}_LOCATION "${__modpath}" CACHE PATH "" FORCE)
          endif()
        endif()
      endforeach()
    endif()
  endforeach()
  iva_clear_vars(__ivamodules __directories_observed __path __modpath __pathIdx)

  # resolve dependencies
  __iva_flatten_module_dependencies()

  # create modules
  set(IVALIB_INITIAL_PASS OFF PARENT_SCOPE)
  set(IVALIB_INITIAL_PASS OFF)
  foreach(m ${IVALIB_MODULES_BUILD})
    if(m MATCHES "^ivalib_")
      string(REGEX REPLACE "^ivalib_" "" __shortname "${m}")
      add_subdirectory("${IVALIB_MODULE_${m}_LOCATION}" "${CMAKE_CURRENT_BINARY_DIR}/${__shortname}")
    endif()
  endforeach()
  unset(__shortname)
endmacro()

# setup include paths for the list of passed modules
macro(iva_include_modules)
#  message("We are in iva_include_modules and the module is "${the_module})
  foreach(d ${ARGN})
 #   message("The value of d is :" ${d})
    if(d MATCHES "^ivalib_" AND HAVE_${d})
      if (EXISTS "${IVALIB_MODULE_${d}_LOCATION}/include")
        iva_include_directories("${IVALIB_MODULE_${d}_LOCATION}/include")
      endif()
      #TODO : find a way to add inculde folders of the required modules
    elseif(EXISTS "${d}")
      iva_include_directories("${d}")
    endif()
  endforeach()
endmacro()

# setup include paths for the list of passed modules and recursively add dependent modules
macro(iva_include_modules_recurse)
  foreach(d ${ARGN})
    if(d MATCHES "^ivalib_" AND HAVE_${d})
      if (EXISTS "${IVALIB_MODULE_${d}_LOCATION}/include")
        iva_include_directories("${IVALIB_MODULE_${d}_LOCATION}/include")
      endif()
#      MESSAGE("d is equal to ${d} and the 3P_include is ${IVALIB_MODULE_${d}_3P_INCLUDE}")
      iva_include_directories("${IVALIB_MODULE_${d}_3P_INCLUDE}")
      if(IVALIB_MODULE_${d}_DEPS)
        iva_include_modules_recurse(${IVALIB_MODULE_${d}_DEPS})
      endif()
    elseif(EXISTS "${d}")
      iva_include_directories("${d}")
    endif()
  endforeach()
endmacro()


# sets header and source files for the current module
# NB: all files specified as headers will be installed
# Usage:
# iva_set_module_sources([HEADERS] <list of files> [SOURCES] <list of files>)
macro(iva_set_module_sources)
  set(IVALIB_MODULE_${the_module}_HEADERS "")
  set(IVALIB_MODULE_${the_module}_SOURCES "")

  foreach(f "HEADERS" ${ARGN})
    if(f STREQUAL "HEADERS" OR f STREQUAL "SOURCES")
      set(__filesvar "IVALIB_MODULE_${the_module}_${f}")
    else()
      list(APPEND ${__filesvar} "${f}")
    endif()
  endforeach()

   
   # use full paths for module to be independent from the module location
   iva_convert_to_full_paths(IVALIB_MODULE_${the_module}_HEADERS)
#   MESSAGE("The IVALIB_MODULE_${the_module}_SOURCES are " ${IVALIB_MODULE_${the_module}_SOURCES}) 
#   MESSAGE("The IVALIB_MODULE_${the_module}_HEADERS are " {${IVALIB_MODULE_${the_module}_HEADERS}) 
   
   set(IVALIB_MODULE_${the_module}_HEADERS ${IVALIB_MODULE_${the_module}_HEADERS} CACHE INTERNAL "List of header files for ${the_module}")
   set(IVALIB_MODULE_${the_module}_SOURCES ${IVALIB_MODULE_${the_module}_SOURCES} CACHE INTERNAL "List of source files for ${the_module}")
 endmacro()


 # finds and sets headers and sources for the standard IVALib module
 # Usage:
 # iva_glob_module_sources(<extra sources&headers in the same format as used in ocv_set_module_sources>)
 macro(iva_glob_module_sources)
   file( GLOB lib_srcs src/*.cpp)
   #message("this is the variable realname : ${realname}")
   file( GLOB lib_hdrs include/IVALib/${realname}/*.hpp)

   source_group("Src" FILES ${lib_srcs} )
   source_group("Include" FILES ${lib_hdrs})
  
   iva_set_module_sources(${ARGN} HEADERS ${lib_hdrs} ${lib_hdrs_detail} SOURCES ${lib_srcs} ${lib_int_hdrs})
  
 endmacro()

# creates IVALib module in current folder
# creates new target, configures standard dependencies, compilers flags, install rules
# Usage:
#   iva_create_module(<extra link dependencies>)
#   iva_create_module(SKIP_LINK)
macro(iva_create_module)
  add_library(${the_module} SHARED ${IVALIB_MODULE_${the_module}_HEADERS} ${IVALIB_MODULE_${the_module}_SOURCES})
    
  target_link_libraries(${the_module} ${IVALIB_MODULE_${the_module}_DEPS} ${IVALIB_MODULE_${the_module}_DEPS_EXT} ${CORE_LIBRARIES} ${ARGN})


  if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(${the_module} PROPERTIES FOLDER "modules")
  endif()


  if(BUILD_SHARED_LIBS)
    if(MSVC)
#      set_target_properties(${the_module} PROPERTIES DEFINE_SYMBOL CVAPI_EXPORTS)
    else()
#      add_definitions(-DCVAPI_EXPORTS)
    endif()
  endif()

  if(MSVC)
    if(CMAKE_CROSSCOMPILING)
      set_target_properties(${the_module} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:secchk")
    endif()
    set_target_properties(${the_module} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libc /DEBUG")
  endif()

  install(TARGETS ${the_module}
    RUNTIME DESTINATION bin COMPONENT main
    LIBRARY DESTINATION ${IVALIB_LIB_INSTALL_PATH} COMPONENT main
    ARCHIVE DESTINATION ${IVALIB_LIB_INSTALL_PATH} COMPONENT main
    )

     # message("The variable realname is : ${realname}")
  
  # install headers
    foreach(hdr ${IVALIB_MODULE_${the_module}_HEADERS})
      install(FILES ${hdr} DESTINATION "${IVALIB_INCLUDE_INSTALL_PATH}/${realname}/" COMPONENT main)
    endforeach()
endmacro()

# setup include path for IVALib headers for specified module
# iva_module_include_directories(<extra include directories/extra include modules>)
macro(iva_module_include_directories)
  iva_include_directories("${IVALIB_MODULE_${the_module}_LOCATION}/include"
    #"${IVALIB_MODULE_${the_module}_LOCATION}/src"
    #"${CMAKE_CURRENT_BINARY_DIR}" # for precompiled headers
    )
  set(IVALIB_MODULE_${the_module}_3P_INCLUDE "${argn}" CACHE INTERNAL "set of 3P include directories")
  iva_include_modules_recurse(${IVALIB_MODULE_${the_module}_DEPS} ${ARGN})
endmacro()

# short command for adding simple OpenCV module
# see iva_add_module for argument details
# Usage:
# ocv_define_module(module_name  [INTERNAL] [REQUIRED] [<list of dependencies>] [OPTIONAL <list of optional dependencies>])
macro(iva_define_module module_name)
  iva_add_module(${module_name} ${ARGN})
  iva_glob_module_sources()
  iva_module_include_directories()
  iva_create_module()
endmacro()

# ensures that all passed modules are available
# sets OCV_DEPENDENCIES_FOUND variable to TRUE/FALSE
macro(iva_check_dependencies)
  set(IVALIB_DEPENDENCIES_FOUND TRUE)
  foreach(d ${ARGN})
    if(d MATCHES "^ivalib_[^ ]+$" AND NOT HAVE_${d})
      set(IVALIB_DEPENDENCIES_FOUND FALSE)
      break()
    endif()
  endforeach()
endmacro()
