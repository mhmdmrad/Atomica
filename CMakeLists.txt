cmake_minimum_required(VERSION 3.16)
project(Atomica LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ─── SOURCES ───────────────────────────────────────────────────────
file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

file(GLOB IMGUI_CORE
  ${CMAKE_SOURCE_DIR}/include/imgui/imgui.cpp
  ${CMAKE_SOURCE_DIR}/include/imgui/imgui_draw.cpp
  ${CMAKE_SOURCE_DIR}/include/imgui/imgui_tables.cpp
  ${CMAKE_SOURCE_DIR}/include/imgui/imgui_widgets.cpp
)
file(GLOB IMGUI_BACKENDS
  ${CMAKE_SOURCE_DIR}/include/imgui/backends/imgui_impl_glfw.cpp
  ${CMAKE_SOURCE_DIR}/include/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(${PROJECT_NAME}
  ${PROJECT_SOURCES}
  ${IMGUI_CORE}
  ${IMGUI_BACKENDS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/imgui
  ${CMAKE_SOURCE_DIR}/include/imgui/backends
  ${CMAKE_SOURCE_DIR}/src
)

# ─── STATIC LIB DIR ─────────────────────────────────────────────────
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib/Release/x64")

# explicitly grab the absolute .a/.lib files:
find_library(GLFW3_PATH
  NAMES libglfw3 glfw3
  PATHS ${LIB_DIR}
  NO_DEFAULT_PATH
)
find_library(GLEW32S_PATH
  NAMES libglew32s glew32s
  PATHS ${LIB_DIR}
  NO_DEFAULT_PATH
)

if (NOT GLFW3_PATH)
  message(FATAL_ERROR "Could not find libglfw3 (.a or .lib) in ${LIB_DIR}")
endif()
if (NOT GLEW32S_PATH)
  message(FATAL_ERROR "Could not find libglew32s (.a or .lib) in ${LIB_DIR}")
endif()

message(STATUS "Linking GLFW3 = ${GLFW3_PATH}")
message(STATUS "Linking GLEW32S = ${GLEW32S_PATH}")

# ─── OPENGL ─────────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)

# ─── LINK ────────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${GLEW32S_PATH}
    ${GLFW3_PATH}
    OpenGL::GL
	"${LIB_DIR}/libglfw3.a"    # MinGW static GLFW3
	"${LIB_DIR}/glew32s.lib"  # MinGW static GLEW
    opengl32   # ensure it’s after everything
)

# ─── DEFINES ─────────────────────────────────────────────────────────
target_compile_definitions(${PROJECT_NAME} PRIVATE
  GLEW_STATIC
  IMGUI_IMPL_OPENGL_LOADER_GLEW
  _CRT_SECURE_NO_WARNINGS
)

if (WIN32)
  message(STATUS "Building on Windows x64")
endif()
