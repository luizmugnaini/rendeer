cmake_minimum_required(VERSION 3.26)

project(
    rendeer
    VERSION 0.0.1
    LANGUAGES CXX C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)
message(STATUS "Using C++ compiler ${CMAKE_CXX_COMPILER}")

set(
    GP_CXX_FLAGS
    "-std=c++17"
    "-Wall"
    "-Wextra"
    "-pedantic"
    "-Wshadow" # warn the user if a variable declaration shadows a parent variable.
    "-Wnon-virtual-dtor" # warn the user if a class with virtual functions has a
                         # non-virtual destructor. This helps catch hard to
                         # track down memory errors.
    "-Wold-style-cast" # warn for c-style casts
    "-Wcast-align"     # warn for potential performance problem casts
    "-Wunused"         # warn on anything being unused
    "-Woverloaded-virtual" # warn if you overload (not override) a virtual
                           # function
    "-Wpedantic"   # warn if non-standard C++ is used
    "-Wconversion" # warn on type conversions that may lose data
    "-Wsign-conversion"  # warn on sign conversions
    "-Wnull-dereference" # warn if a null dereference is detected
    "-Wdouble-promotion" # warn if float is implicit promoted to double
    "-Wformat=2" # warn on security issues around functions that format output
                 # (ie printf)
    "-Wmisleading-indentation" # warn if indentation implies blocks where blocks
                               # do not exist
)
set(
    GP_SAN_CXX_FLAGS
        "-fsanitize=address" # Enable AddressSanitizer, a fast memory error detector.
                         # Memory access instructions are instrumented to detect
                         # out-of-bounds and use-after-free bugs. The option
                         # enables -fsanitize-address-use-after-scope.
    "-fsanitize=pointer-compare" # Instrument comparison operation (<, <=, >, >=) with
                                 # pointer operands. The option must be combined with
                                 # either -fsanitize=kernel-address or -fsanitize=address
    "-fsanitize=pointer-subtract" # Instrument subtraction with pointer operands.
                                  # The option must be combined with either
                                  # -fsanitize=kernel-address or -fsanitize=address.
    "-fsanitize=shadow-call-stack" # Enable ShadowCallStack, a security enhancement
                                   # mechanism used to protect programs against return
                                   # address overwrites (e.g. stack buffer overflows.)
    "-fsanitize=leak" # Enable LeakSanitizer, a memory leak detector. This option only
                      # matters for linking of executables.
    "-fsanitize=undefined" # Enable UndefinedBehaviorSanitizer.
    "-fstack-protector-strong" # Like -fstack-protector but includes additional functions
                               # to be protected — those that have local array definitions,
                               # or have references to local frame addresses.
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
include(FetchContent)

FetchContent_Declare(
    glfw 
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory("${glfw_SOURCE_DIR}" "${glfw_BINARY_DIR}")
endif()

add_library(
    glad STATIC
    "thirdparty/glad/src/gl.c"
    "thirdparty/glad/include/glad/gl.h"
)
target_include_directories(glad PUBLIC "thirdparty/glad/include")

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
      FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

add_library(base STATIC "src/base/utils.cpp")
target_compile_options(base PRIVATE ${GP_CXX_FLAGS} ${GP_SAN_CXX_FLAGS})
target_include_directories(base PUBLIC "src/base")
target_link_libraries(base PRIVATE ${GP_SAN_CXX_FLAGS} glad glfw)

add_executable(triforceCPU "src/triforceCPU.cpp")
target_compile_options(triforceCPU PRIVATE ${GP_CXX_FLAGS} ${GP_SAN_CXX_FLAGS})
set_target_properties(triforceCPU PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "triforceCPU")
target_link_libraries(triforceCPU PRIVATE ${GP_SAN_CXX_FLAGS} glfw glad base)

add_executable(triforceTransformFeedback "src/triforceTransformFeedback.cpp")
target_compile_options(triforceTransformFeedback PRIVATE ${GP_CXX_FLAGS} ${GP_SAN_CXX_FLAGS})
set_target_properties(triforceTransformFeedback PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "triforceTransformFeedback")
target_link_libraries(triforceTransformFeedback PRIVATE ${GP_SAN_CXX_FLAGS} glfw glad base)

add_executable(rectangle3D "src/rectangle3D.cpp")
target_compile_options(rectangle3D PRIVATE ${GP_CXX_FLAGS} ${GP_SAN_CXX_FLAGS})
set_target_properties(rectangle3D PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "rectangle3D")
target_link_libraries(rectangle3D PRIVATE ${GP_SAN_CXX_FLAGS} glfw glad base)
