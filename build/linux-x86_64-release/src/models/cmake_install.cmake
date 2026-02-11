# Install script for directory: /home/kijetesantakalu/thesis/testBSMPT2/BSMPT/src/models

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/build/linux-x86_64-release/src/models/libModels.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/BSMPT/models" TYPE FILE FILES
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/SMparam.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/IncludeAllModels.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/ClassPotentialOrigin.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/ClassPotentialC2HDM.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/ClassPotentialR2HDM.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/ClassPotentialN2HDM.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/ClassPotentialCxSM.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/ClassPotentialSM.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/ClassPotentialCPintheDark.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/ClassPotentialRxSM.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/ClassPotentialRxSMnoZ2.h"
    "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/include/BSMPT/models/ClassTemplate.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/build/linux-x86_64-release/src/models/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
