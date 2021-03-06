###############################################################################
# CMAKE DEFINITIONS AND setTINGS
#
# Requires the following environmental variables:
#
#   BUILD TYPE      ENV VARIABLE        Description
#   ANDROID         ANDROID_NDK         Android NDK root folder location
#                                           (also required for toolchain file)
#   ANDROID         GRADLE_EXECUTABLE   Location of the Gradle executable
###############################################################################
set(ROOT_PROJECT_NAME Sustain)
set(${ROOT_PROJECT_NAME}_CMAKE_MIN_VERSION 3.11.0)
set(${ROOT_PROJECT_NAME}_CMAKE_POLICY 3.11.0)

cmake_minimum_required(VERSION ${${ROOT_PROJECT_NAME}_CMAKE_MIN_VERSION})
project(${ROOT_PROJECT_NAME} CXX)

include(cmake/cmake-common_logic.cmake)
include(cmake/UseGit.cmake)


 list(APPEND CMAKE_MODULE_PATH   ${PROJECT_SOURCE_DIR}/cmake  ${PROJECT_SOURCE_DIR}/cmake/common )

 message(STATUS "
    Using the following search paths
    CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}
    CMAKE_PREFIX_PATH  = ${CMAKE_PREFIX_PATH}
    CMAKE_FIND_ROOT_PATH = ${CMAKE_FIND_ROOT_PATH}
    ")

 add_subdirectory(projects)
 
 set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT pfc_registry)
 
 ###############################################################################
# Stage Code for IDE Testing
###############################################################################
create_cache_file()
create_stage()
###############################################################################
# Step 3:
# Infrastructure Installation for each library
# your project outputs create a Find${lib}.cmake file
# list them here. This will install them in your sysroot
# so other projects can build off those libraries
###############################################################################
# install(EXPORT lib${ROOT_PROJECT_NAME}
#   NAMESPACE ${ROOT_PROJECT_NAME}::
#   FILE ${ROOT_PROJECT_NAME}.cmake
#   DESTINATION lib/cmake/${ROOT_PROJECT_NAME}
# )

include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/${ROOT_PROJECT_NAME}Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${ROOT_PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "lib/cmake/${ROOT_PROJECT_NAME}"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${ROOT_PROJECT_NAME}ConfigVersion.cmake"
  VERSION "${${ROOT_PROJECT_NAME}_VERSION_MAJOR}.${${ROOT_PROJECT_NAME}_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

 install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/${ROOT_PROJECT_NAME}Config.cmake
   ${Packages}
   DESTINATION lib/cmake/${ROOT_PROJECT_NAME}
 )
