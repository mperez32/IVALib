#------------------------------------------------------------
#Macros and functions used to build IVALib
#------------------------------------------------------------

# Search packages for host system instead of packages for target system
# in case of cross compilation thess macro should be defined by toolchain file
if(NOT COMMAND find_host_package)
  macro(find_host_package)
    find_package(${ARGN})
  endmacro()
endif()
if(NOT COMMAND find_host_program)
  macro(find_host_program)
    find_program(${ARGN})
  endmacro()
endif()

# adds include directories in such way that directories from the IVALib source tree go first
function(iva_include_directories)
  set(__add_before "")
  foreach(dir ${ARGN})
    get_filename_component(__abs_dir "${dir}" ABSOLUTE)
    if("${__abs_dir}" MATCHES "^${IVALIB_SOURCE_DIR}" OR "${__abs_dir}" MATCHES "^${IVALIB_BINARY_DIR}")
      list(APPEND __add_before "${dir}")
    else()
      include_directories(AFTER SYSTEM "${dir}")
    endif()
  endforeach()
  include_directories(BEFORE ${__add_before})
endfunction()

# clears all passed variables
macro(iva_clear_vars)
  foreach(_var ${ARGN})
    unset(${_var} CACHE)
  endforeach()
endmacro()

# Provides an option that the user can optionally select.
# Can accept condition to control when option is available for user.
# Usage:
#   iva_option(<option_variable> "help string describing the option" <initial value or boolean expression> [IF <condition>])
macro(IVA_OPTION variable description value)
  set(__value ${value})
  set(__condition "")
  set(__varname "__value")
  foreach(arg ${ARGN})
    if(arg STREQUAL "IF" OR arg STREQUAL "if")
      set(__varname "__condition")
    else()
      list(APPEND ${__varname} ${arg})
    endif()
  endforeach()
  unset(__varname)
  if("${__condition}" STREQUAL "")
    set(__condition 2 GREATER 1)
  endif()

  if(${__condition})
    if("${__value}" MATCHES ";")
      if(${__value})
        option(${variable} "${description}" ON)
      else()
        option(${variable} "${description}" OFF)
      endif()
    elseif(DEFINED ${__value})
      if(${__value})
        option(${variable} "${description}" ON)
      else()
        option(${variable} "${description}" OFF)
      endif()
    else()
      option(${variable} "${description}" ${__value})
    endif()
  else()
    unset(${variable} CACHE)
  endif()
  unset(__condition)
  unset(__value)
endmacro()

# get absolute path with symlinks resolved
macro(iva_get_real_path VAR PATHSTR)
  if(CMAKE_VERSION VERSION_LESS 2.8)
    get_filename_component(${VAR} "${PATHSTR}" ABSOLUTE)
  else()
    get_filename_component(${VAR} "${PATHSTR}" REALPATH)
  endif()
endmacro()

# stable & safe duplicates removal macro
macro(iva_list_unique __lst)
  if(${__lst})
    list(REMOVE_DUPLICATES ${__lst})
  endif()
endmacro()

# safe list reversal macro
macro(iva_list_reverse __lst)
  if(${__lst})
    list(REVERSE ${__lst})
  endif()
endmacro()


# safe list sorting macro
macro(iva_list_sort __lst)
  if(${__lst})
    list(SORT ${__lst})
  endif()
endmacro()

# gets and removes the first element from list
macro(iva_list_pop_front LST VAR)
  if(${LST})
    list(GET ${LST} 0 ${VAR})
    list(REMOVE_AT ${LST} 0)
  else()
    set(${VAR} "")
  endif()
endmacro()

# Macros that checks if module have been installed.
# After it adds module to build and define
# constants passed as second arg
macro(CHECK_MODULE module_name define)
  set(${define} 0)
  if(PKG_CONFIG_FOUND)
    set(ALIAS               ALIASOF_${module_name})
    set(ALIAS_FOUND                 ${ALIAS}_FOUND)
    set(ALIAS_INCLUDE_DIRS   ${ALIAS}_INCLUDE_DIRS)
    set(ALIAS_LIBRARY_DIRS   ${ALIAS}_LIBRARY_DIRS)
    set(ALIAS_LIBRARIES         ${ALIAS}_LIBRARIES)

    PKG_CHECK_MODULES(${ALIAS} ${module_name})

    if(${ALIAS_FOUND})
      set(${define} 1)
      foreach(P "${ALIAS_INCLUDE_DIRS}")
        if(${P})
          list(APPEND PLOTTER_INCLUDE_DIRS ${${P}})
        endif()
      endforeach()

      foreach(P "${ALIAS_LIBRARY_DIRS}")
        if(${P})
          list(APPEND PLOTTER_LIBRARY_DIRS ${${P}})
        endif()
      endforeach()

      list(APPEND PLOTTER_LIBRARIES ${${ALIAS_LIBRARIES}})
    endif()
  endif()
endmacro()

# convert list of paths to full paths
macro(iva_convert_to_full_paths VAR)
  if(${VAR})
    set(__tmp "")
    foreach(path ${${VAR}})
      get_filename_component(${VAR} "${path}" ABSOLUTE)
      list(APPEND __tmp "${${VAR}}")
    endforeach()
    set(${VAR} ${__tmp})
    unset(__tmp)
  endif()
endmacro()

# remove all matching elements from the list
macro(iva_list_filterout lst regex)
  foreach(item ${${lst}})
    if(item MATCHES "${regex}")
      list(REMOVE_ITEM ${lst} "${item}")
    endif()
  endforeach()
endmacro()

