cmake_minimum_required(VERSION 3.8)
project(BMG-Keylogger)
# Project Version
set(PROJECT_VERSION "1.0")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Setting compile definitions
add_compile_definitions(VERSION="${PROJECT_VERSION}")
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

# Include directories
include_directories(include)

# Add external libraries 
add_subdirectory(external/raylib)

# Add ImGui source files
add_library(ImGui STATIC
  external/imgui/imgui.cpp
  external/imgui/imgui_draw.cpp
  external/imgui/imgui_demo.cpp
  external/imgui/imgui_tables.cpp
  external/imgui/imgui_widgets.cpp
)
target_include_directories(ImGui PUBLIC
  external/imgui
  external/imgui/backends
  external/raylib/src
)

# Add rlimgui source files
add_library(rlImGui STATIC
  external/rlimgui/rlImGui.cpp
)
target_include_directories(rlImGui PUBLIC
  external/rlImGui
  external/raylib/src
)
target_link_libraries(rlImGui PRIVATE raylib ImGui)


# Organize source files into "Source Files"
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c src/entitycomponentsystem/*.c src/entitycomponentsystem/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp src/entitycomponentsystem/*.h src/entitycomponentsystem/*.hpp)

source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})

# Add source files
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} resource.rc)

# Check if the generator is Visual Studio
if (CMAKE_GENERATOR MATCHES "Visual Studio")
  message(STATUS "Setting up for Visual Studio...")
  # Set multithreading
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  # Set the Working Directory for each Config 
  set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

  # Set the startup project for Visual Studio
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

# Check if we're building on Windows
if(WIN32)
    # Apply additional linker options
    if(MSVC)
      target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-mwindows>)
    endif()
endif()

if(WIN32 AND MSVC)
  target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>)
endif()

# Link libraries 
target_link_libraries(${PROJECT_NAME} PRIVATE raylib rlImGui ImGui)

# set the source directory of resources
set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)

# Copy the contents of the res folder to the build directory if there is one
if(EXISTS ${RES_DIR})
  message(STATUS "Resource folder fond: ${RES_DIR}")
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RES_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying resources to the build directory..."
  )
else()
  # Skipping copying resources
  message(WARNING "Resource folder not found: ${RES_DIR}. Skipping sesource copy.")
endif()
