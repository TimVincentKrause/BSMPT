# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/cmaes-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${cmaes_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${cmaes_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET libcmaes::cmaes)
    add_library(libcmaes::cmaes INTERFACE IMPORTED)
    message(${cmaes_MESSAGE_MODE} "Conan: Target declared 'libcmaes::cmaes'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/cmaes-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()