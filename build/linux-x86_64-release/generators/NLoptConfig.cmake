########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(NLopt_FIND_QUIETLY)
    set(NLopt_MESSAGE_MODE VERBOSE)
else()
    set(NLopt_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/NLoptTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${nlopt_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(NLopt_VERSION_STRING "2.9.1")
set(NLopt_INCLUDE_DIRS ${nlopt_INCLUDE_DIRS_RELEASE} )
set(NLopt_INCLUDE_DIR ${nlopt_INCLUDE_DIRS_RELEASE} )
set(NLopt_LIBRARIES ${nlopt_LIBRARIES_RELEASE} )
set(NLopt_DEFINITIONS ${nlopt_DEFINITIONS_RELEASE} )


# Only the last installed configuration BUILD_MODULES are included to avoid the collision
foreach(_BUILD_MODULE ${nlopt_BUILD_MODULES_PATHS_RELEASE} )
    message(${NLopt_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


