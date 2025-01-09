#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.16...3.27)
project(Opt2)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

# Don't forget Julia headers
include_directories(${PROJECT_SOURCE_DIR}/include $ENV{JULIA_DIR}/include/julia)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/julia/*.jl)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(Opt2 opt2.cc ${sources} ${headers})

# This line is important:
target_link_directories(Opt2 PRIVATE $ENV{JULIA_DIR}/lib/julia $ENV{JULIA_DIR}/lib)
target_link_libraries(Opt2 ${Geant4_LIBRARIES} julia)

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build the app. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(OPTICAL2_SCRIPTS
  julia/steering.jl
  julia/custom_module.jl
  example.in
  gui.mac
  init_vis.mac
  run1.mac
  run2.mac
  run3.mac  
  vis.mac
  )

foreach(_script ${OPTICAL2_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS Opt2 DESTINATION bin)
