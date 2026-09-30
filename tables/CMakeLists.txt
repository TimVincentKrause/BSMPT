# tables/CMakeLists.txt – FINAL WORKING VERSION 2025

# Dummy custom target (not a library → no add_library → no conflicts)
add_custom_target(TablesDeploy ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory 
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tables/potential"
    COMMAND ${CMAKE_COMMAND} -E make_directory 
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tables/integration"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/potential"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tables/potential"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/integration"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tables/integration"
    COMMENT "Deploying tables to bin/tables/"
    VERBATIM
)

# Force every executable to wait for tables
add_dependencies(ThermalFunctions TablesDeploy)