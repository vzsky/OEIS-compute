cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

execute_process(
  COMMAND ${CMAKE_COMMAND} -E create_symlink
          ${CMAKE_BINARY_DIR}/compile_commands.json
          ${CMAKE_SOURCE_DIR}/compile_commands.json
)

project(OEIS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

file(GLOB children RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/*)

foreach(child ${children})
  set(child_path "${CMAKE_SOURCE_DIR}/${child}")
  if(IS_DIRECTORY ${child_path})
    # Check if the subdirectory contains a CMakeLists.txt
    if(EXISTS "${child_path}/CMakeLists.txt")
      message(STATUS "Adding subdirectory: ${child}")
      add_subdirectory(${child})
    endif()
  endif()
endforeach()
