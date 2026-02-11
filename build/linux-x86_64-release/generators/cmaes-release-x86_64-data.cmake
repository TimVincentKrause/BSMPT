########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(cmaes_COMPONENT_NAMES "")
if(DEFINED cmaes_FIND_DEPENDENCY_NAMES)
  list(APPEND cmaes_FIND_DEPENDENCY_NAMES Eigen3)
  list(REMOVE_DUPLICATES cmaes_FIND_DEPENDENCY_NAMES)
else()
  set(cmaes_FIND_DEPENDENCY_NAMES Eigen3)
endif()
set(Eigen3_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(cmaes_PACKAGE_FOLDER_RELEASE "/home/kijetesantakalu/.conan2/p/b/cmaes7c31999389134/p")
set(cmaes_BUILD_MODULES_PATHS_RELEASE )


set(cmaes_INCLUDE_DIRS_RELEASE "${cmaes_PACKAGE_FOLDER_RELEASE}/include")
set(cmaes_RES_DIRS_RELEASE )
set(cmaes_DEFINITIONS_RELEASE )
set(cmaes_SHARED_LINK_FLAGS_RELEASE )
set(cmaes_EXE_LINK_FLAGS_RELEASE )
set(cmaes_OBJECTS_RELEASE )
set(cmaes_COMPILE_DEFINITIONS_RELEASE )
set(cmaes_COMPILE_OPTIONS_C_RELEASE )
set(cmaes_COMPILE_OPTIONS_CXX_RELEASE )
set(cmaes_LIB_DIRS_RELEASE "${cmaes_PACKAGE_FOLDER_RELEASE}/lib")
set(cmaes_BIN_DIRS_RELEASE )
set(cmaes_LIBRARY_TYPE_RELEASE STATIC)
set(cmaes_IS_HOST_WINDOWS_RELEASE 0)
set(cmaes_LIBS_RELEASE cmaes)
set(cmaes_SYSTEM_LIBS_RELEASE )
set(cmaes_FRAMEWORK_DIRS_RELEASE )
set(cmaes_FRAMEWORKS_RELEASE )
set(cmaes_BUILD_DIRS_RELEASE )
set(cmaes_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(cmaes_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${cmaes_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${cmaes_COMPILE_OPTIONS_C_RELEASE}>")
set(cmaes_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${cmaes_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${cmaes_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${cmaes_EXE_LINK_FLAGS_RELEASE}>")


set(cmaes_COMPONENTS_RELEASE )