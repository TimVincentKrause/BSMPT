########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(cmaes_FIND_QUIETLY)
    set(cmaes_MESSAGE_MODE VERBOSE)
else()
    set(cmaes_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmaesTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${cmaes_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(cmaes_VERSION_STRING "0.10.0")
set(cmaes_INCLUDE_DIRS ${cmaes_INCLUDE_DIRS_RELEASE} )
set(cmaes_INCLUDE_DIR ${cmaes_INCLUDE_DIRS_RELEASE} )
set(cmaes_LIBRARIES ${cmaes_LIBRARIES_RELEASE} )
set(cmaes_DEFINITIONS ${cmaes_DEFINITIONS_RELEASE} )


# Only the last installed configuration BUILD_MODULES are included to avoid the collision
foreach(_BUILD_MODULE ${cmaes_BUILD_MODULES_PATHS_RELEASE} )
    message(${cmaes_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


