#----------------------------------------------------------------------------
# Compile command, from inside build directory:
# cmake -DGeant4_DIR=~/Applications/geant4.10/lib/ ../ShortBaselineG4MC/
# make -j4

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(ShortBaselineG4MC)


#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/src/*/*.cc )
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh ${PROJECT_SOURCE_DIR}/include/*/*.hh)
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/include/Physics
                    ${PROJECT_SOURCE_DIR}/include/Geometry
                    ${PROJECT_SOURCE_DIR}/include/EventGen
                    ${PROJECT_SOURCE_DIR}/include/Output
                    ${PROJECT_SOURCE_DIR}/include/Tracking
                    $ENV{CRYHOME}/src)
link_directories($ENV{CRYHOME}/lib)

#-------------------------------
# ROOT dependencies
#-------------------------------

######## locate ROOT install, headers, includes
include(FindROOT.cmake)
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
include(${Geant4_USE_FILE})
Message("Geant4_USE_FILE = ${Geant4_USE_FILE}")

##############
# Output paths
##############

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")


###############
# TObject subclasses dictionary & ROOT objects shared library
###############

set(SB_G4_LINKDEF ${PROJECT_SOURCE_DIR}/include/Output/EventLinkDef.h)
set(SB_G4_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/EventLib_Dict.cxx)
MESSAGE("Generating dictionary ${SB_G4_DICTIONARY}")
set(DICTFILES "${PROJECT_SOURCE_DIR}/include/Output/Event.hh")
ROOT_GENERATE_DICTIONARY("${DICTFILES}" "${SB_G4_LINKDEF}" "${SB_G4_DICTIONARY}" "")
LIST(APPEND sources ${SB_G4_DICTIONARY})

add_library(EventLib SHARED ${sources} ${headers})
target_link_libraries(EventLib CRY ${ROOT_LIBRARIES} ${Geant4_LIBRARIES})
SET(SB_G4_LIBRARY_PROPERTIES
		${SB_G4_LIBRARY_PROPERTIES}
		VERSION "0"
		SOVERSION "0"
		SUFFIX ".so"
)
set_target_properties(EventLib PROPERTIES ${SB_G4_LIBRARY_PROPERTIES})
install(TARGETS EventLib DESTINATION ${LIBRARY_OUTPUT_PATH})
link_directories(${LIBRARY_OUTPUT_PATH})


#################
# Executables
#################

add_executable(PROSPECT-G4 PROSPECT-G4.cc)
target_link_libraries(PROSPECT-G4 EventLib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

install(TARGETS PROSPECT-G4 DESTINATION ${EXECUTABLE_OUTPUT_PATH})


#################
# Build settings
#################

#set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_BUILD_TYPE release)

