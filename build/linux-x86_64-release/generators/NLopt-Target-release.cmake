# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(nlopt_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(nlopt_FRAMEWORKS_FOUND_RELEASE "${nlopt_FRAMEWORKS_RELEASE}" "${nlopt_FRAMEWORK_DIRS_RELEASE}")

set(nlopt_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET nlopt_DEPS_TARGET)
    add_library(nlopt_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET nlopt_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${nlopt_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${nlopt_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### nlopt_DEPS_TARGET to all of them
conan_package_library_targets("${nlopt_LIBS_RELEASE}"    # libraries
                              "${nlopt_LIB_DIRS_RELEASE}" # package_libdir
                              "${nlopt_BIN_DIRS_RELEASE}" # package_bindir
                              "${nlopt_LIBRARY_TYPE_RELEASE}"
                              "${nlopt_IS_HOST_WINDOWS_RELEASE}"
                              nlopt_DEPS_TARGET
                              nlopt_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "nlopt"    # package_name
                              "${nlopt_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${nlopt_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Release ########################################

    ########## COMPONENT NLopt::nlopt #############

        set(nlopt_NLopt_nlopt_FRAMEWORKS_FOUND_RELEASE "")
        conan_find_apple_frameworks(nlopt_NLopt_nlopt_FRAMEWORKS_FOUND_RELEASE "${nlopt_NLopt_nlopt_FRAMEWORKS_RELEASE}" "${nlopt_NLopt_nlopt_FRAMEWORK_DIRS_RELEASE}")

        set(nlopt_NLopt_nlopt_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET nlopt_NLopt_nlopt_DEPS_TARGET)
            add_library(nlopt_NLopt_nlopt_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET nlopt_NLopt_nlopt_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${nlopt_NLopt_nlopt_FRAMEWORKS_FOUND_RELEASE}>
                     $<$<CONFIG:Release>:${nlopt_NLopt_nlopt_SYSTEM_LIBS_RELEASE}>
                     $<$<CONFIG:Release>:${nlopt_NLopt_nlopt_DEPENDENCIES_RELEASE}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'nlopt_NLopt_nlopt_DEPS_TARGET' to all of them
        conan_package_library_targets("${nlopt_NLopt_nlopt_LIBS_RELEASE}"
                              "${nlopt_NLopt_nlopt_LIB_DIRS_RELEASE}"
                              "${nlopt_NLopt_nlopt_BIN_DIRS_RELEASE}" # package_bindir
                              "${nlopt_NLopt_nlopt_LIBRARY_TYPE_RELEASE}"
                              "${nlopt_NLopt_nlopt_IS_HOST_WINDOWS_RELEASE}"
                              nlopt_NLopt_nlopt_DEPS_TARGET
                              nlopt_NLopt_nlopt_LIBRARIES_TARGETS
                              "_RELEASE"
                              "nlopt_NLopt_nlopt"
                              "${nlopt_NLopt_nlopt_NO_SONAME_MODE_RELEASE}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET NLopt::nlopt
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${nlopt_NLopt_nlopt_OBJECTS_RELEASE}>
                     $<$<CONFIG:Release>:${nlopt_NLopt_nlopt_LIBRARIES_TARGETS}>
                     )

        if("${nlopt_NLopt_nlopt_LIBS_RELEASE}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET NLopt::nlopt
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         nlopt_NLopt_nlopt_DEPS_TARGET)
        endif()

        set_property(TARGET NLopt::nlopt APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Release>:${nlopt_NLopt_nlopt_LINKER_FLAGS_RELEASE}>)
        set_property(TARGET NLopt::nlopt APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Release>:${nlopt_NLopt_nlopt_INCLUDE_DIRS_RELEASE}>)
        set_property(TARGET NLopt::nlopt APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Release>:${nlopt_NLopt_nlopt_LIB_DIRS_RELEASE}>)
        set_property(TARGET NLopt::nlopt APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Release>:${nlopt_NLopt_nlopt_COMPILE_DEFINITIONS_RELEASE}>)
        set_property(TARGET NLopt::nlopt APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Release>:${nlopt_NLopt_nlopt_COMPILE_OPTIONS_RELEASE}>)


    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET NLopt::nlopt APPEND PROPERTY INTERFACE_LINK_LIBRARIES NLopt::nlopt)

########## For the modules (FindXXX)
set(nlopt_LIBRARIES_RELEASE NLopt::nlopt)
