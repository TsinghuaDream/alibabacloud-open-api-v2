find_package(darabonba_core QUIET)

if(darabonba_core_FOUND)
  message(STATUS "Found darabonba_core version ${darabonba_core_VERSION}")
else()
  message(STATUS "darabonba_core could not be located, Building darabonba_core instead.")
  include(FetchContent)

  # Try to use local source first if available
  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../tea-cpp/CMakeLists.txt")
    message(STATUS "Using local tea-cpp source")
    FetchContent_Declare(
      _darabonba_core
      SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../tea-cpp"
      CMAKE_ARGS
        -DENABLE_UNIT_TESTS:BOOL="0")
  else()
    FetchContent_Declare(
      _darabonba_core
      GIT_REPOSITORY https://github.com/TsinghuaDream/tea-cpp.git
      GIT_TAG master
      CMAKE_ARGS
        -DENABLE_UNIT_TESTS:BOOL="0")
  endif()

  FetchContent_MakeAvailable(_darabonba_core)
  message(STATUS "Added darabonba_core source : ${_darabonba_core_SOURCE_DIR}")
  message(STATUS "Added darabonba_core binary : ${_darabonba_core_BINARY_DIR}")

  target_include_directories(${PROJECT_NAME} PRIVATE "${_darabonba_core_SOURCE_DIR}/include")
endif()