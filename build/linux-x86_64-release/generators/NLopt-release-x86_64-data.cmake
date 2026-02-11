########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND nlopt_COMPONENT_NAMES NLopt::nlopt)
list(REMOVE_DUPLICATES nlopt_COMPONENT_NAMES)
if(DEFINED nlopt_FIND_DEPENDENCY_NAMES)
  list(APPEND nlopt_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES nlopt_FIND_DEPENDENCY_NAMES)
else()
  set(nlopt_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(nlopt_PACKAGE_FOLDER_RELEASE "/home/kijetesantakalu/.conan2/p/b/nlopt9de5b41bc9d48/p")
set(nlopt_BUILD_MODULES_PATHS_RELEASE )


set(nlopt_INCLUDE_DIRS_RELEASE "${nlopt_PACKAGE_FOLDER_RELEASE}/include")
set(nlopt_RES_DIRS_RELEASE )
set(nlopt_DEFINITIONS_RELEASE )
set(nlopt_SHARED_LINK_FLAGS_RELEASE )
set(nlopt_EXE_LINK_FLAGS_RELEASE )
set(nlopt_OBJECTS_RELEASE )
set(nlopt_COMPILE_DEFINITIONS_RELEASE )
set(nlopt_COMPILE_OPTIONS_C_RELEASE )
set(nlopt_COMPILE_OPTIONS_CXX_RELEASE )
set(nlopt_LIB_DIRS_RELEASE "${nlopt_PACKAGE_FOLDER_RELEASE}/lib")
set(nlopt_BIN_DIRS_RELEASE )
set(nlopt_LIBRARY_TYPE_RELEASE STATIC)
set(nlopt_IS_HOST_WINDOWS_RELEASE 0)
set(nlopt_LIBS_RELEASE nlopt)
set(nlopt_SYSTEM_LIBS_RELEASE m stdc++)
set(nlopt_FRAMEWORK_DIRS_RELEASE )
set(nlopt_FRAMEWORKS_RELEASE )
set(nlopt_BUILD_DIRS_RELEASE )
set(nlopt_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(nlopt_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${nlopt_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${nlopt_COMPILE_OPTIONS_C_RELEASE}>")
set(nlopt_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${nlopt_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${nlopt_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${nlopt_EXE_LINK_FLAGS_RELEASE}>")


set(nlopt_COMPONENTS_RELEASE NLopt::nlopt)
########### COMPONENT NLopt::nlopt VARIABLES ############################################

set(nlopt_NLopt_nlopt_INCLUDE_DIRS_RELEASE "${nlopt_PACKAGE_FOLDER_RELEASE}/include")
set(nlopt_NLopt_nlopt_LIB_DIRS_RELEASE "${nlopt_PACKAGE_FOLDER_RELEASE}/lib")
set(nlopt_NLopt_nlopt_BIN_DIRS_RELEASE )
set(nlopt_NLopt_nlopt_LIBRARY_TYPE_RELEASE STATIC)
set(nlopt_NLopt_nlopt_IS_HOST_WINDOWS_RELEASE 0)
set(nlopt_NLopt_nlopt_RES_DIRS_RELEASE )
set(nlopt_NLopt_nlopt_DEFINITIONS_RELEASE )
set(nlopt_NLopt_nlopt_OBJECTS_RELEASE )
set(nlopt_NLopt_nlopt_COMPILE_DEFINITIONS_RELEASE )
set(nlopt_NLopt_nlopt_COMPILE_OPTIONS_C_RELEASE "")
set(nlopt_NLopt_nlopt_COMPILE_OPTIONS_CXX_RELEASE "")
set(nlopt_NLopt_nlopt_LIBS_RELEASE nlopt)
set(nlopt_NLopt_nlopt_SYSTEM_LIBS_RELEASE m stdc++)
set(nlopt_NLopt_nlopt_FRAMEWORK_DIRS_RELEASE )
set(nlopt_NLopt_nlopt_FRAMEWORKS_RELEASE )
set(nlopt_NLopt_nlopt_DEPENDENCIES_RELEASE )
set(nlopt_NLopt_nlopt_SHARED_LINK_FLAGS_RELEASE )
set(nlopt_NLopt_nlopt_EXE_LINK_FLAGS_RELEASE )
set(nlopt_NLopt_nlopt_NO_SONAME_MODE_RELEASE FALSE)

# COMPOUND VARIABLES
set(nlopt_NLopt_nlopt_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${nlopt_NLopt_nlopt_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${nlopt_NLopt_nlopt_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${nlopt_NLopt_nlopt_EXE_LINK_FLAGS_RELEASE}>
)
set(nlopt_NLopt_nlopt_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${nlopt_NLopt_nlopt_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${nlopt_NLopt_nlopt_COMPILE_OPTIONS_C_RELEASE}>")