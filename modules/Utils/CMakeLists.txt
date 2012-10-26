set(the_description "Utils modules holds various inline functions that are used for basic procedures")
iva_add_module(Utils ${CORE_LIBRARIES} )
iva_module_include_directories(${CORE_INCLUDE_DIRECTORIES})
iva_include_directories(${PLOTTER_INCLUDE_DIRS})
iva_glob_module_sources()

#do not create library since these are only header files with inline functions
#Install the headers in the appropriate folder
foreach(hdr ${IVALIB_MODULE_${the_module}_HEADERS})
  install(FILES ${hdr} DESTINATION "${IVALIB_INCLUDE_INSTALL_PATH}/${realname}/" COMPONENT main)
endforeach()
