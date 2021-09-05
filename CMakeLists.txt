cmake_minimum_required (VERSION 3.8)

project(VoxelBasedGI)

set(NAME "VoxelBasedGI")

# General configuration
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/modules)
set(LLVM_ENABLE_RTTI ON) 
set(LLVM_ENABLE_EH ON)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# Find and link library directories
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

set(LIBS ${GLFW3_LIBRARY} opengl32 ${ASSIMP_LIBRARIES} Winmm.lib)

link_directories(${CMAKE_SOURCE_DIR}/Dependancies/Win64_Lib)

# Add more libraries
add_library(STB_IMAGE "${CMAKE_SOURCE_DIR}/VoxelBasedGI/src/stb_image stuff.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "${CMAKE_SOURCE_DIR}/VoxelBasedGI/src/glad.c")
set(LIBS ${LIBS} GLAD)

# Collect source and shader files
file(GLOB_RECURSE SOURCE
            "${CMAKE_SOURCE_DIR}/VoxelBasedGI/src/*.cpp"
             "${CMAKE_SOURCE_DIR}/VoxelBasedGI/src/*.c"
)
file(GLOB HEADERS    
            "${CMAKE_SOURCE_DIR}/VoxelBasedGI/includes/*.h"
)
file(GLOB_RECURSE SHADERS    
            "${CMAKE_SOURCE_DIR}/Shaders/*.*"
)
# Collect imgui stuff
file(GLOB IMGUI_SOURCE 
"${CMAKE_SOURCE_DIR}/VoxelBasedGI/includes/imgui/*.h"
"${CMAKE_SOURCE_DIR}/VoxelBasedGI/includes/imgui/*.cpp"
"${CMAKE_SOURCE_DIR}/VoxelBasedGI/includes/imgui/backends/*.h"
"${CMAKE_SOURCE_DIR}/VoxelBasedGI/includes/imgui/backends/*.cpp")

add_executable(${NAME} ${SOURCE} ${HEADERS} ${SHADERS} ${IMGUI_SOURCE})
 
# Configure directories
set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Debug")

include_directories(${CMAKE_SOURCE_DIR}/VoxelBasedGI/includes
${CMAKE_SOURCE_DIR}/VoxelBasedGI/includes/imgui
${CMAKE_SOURCE_DIR}/VoxelBasedGI/includes/imgui/backends
${CMAKE_SOURCE_DIR}/configuration
${CMAKE_SOURCE_DIR}/Dependancies
${ASSIMP_INCLUDE_DIR}
${GLFW3_INCLUDE_DIR}
${GLM_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${NAME} ${LIBS})

# Copy libs, models and everything else into the output directory
file(GLOB LIB_FILES    
            "${CMAKE_SOURCE_DIR}/Dependancies/Win64_Lib/*.lib"
            "${CMAKE_SOURCE_DIR}/Dependancies/Win64_Lib/*.dll"
)

install(DIRECTORY   "${CMAKE_SOURCE_DIR}/Textures"
"${CMAKE_SOURCE_DIR}/Shaders" "${CMAKE_SOURCE_DIR}/Models"
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
install(FILES   ${LIB_FILES}
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})