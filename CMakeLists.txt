cmake_minimum_required(VERSION 3.15) # JUCE requires at least 3.15
project(Ray_Marcher)
set(CMAKE_CXX_STANDARD 17)

## ~ CONFIGURE DEPENDENCIES ~
# Set versions of dependencies
set(GLFW_VERSION 3.3.9)
set(GLM_VERSION 1.0.1)
set(JUCE_VERSION 7.0.9) # Latest stable version

# Do not build other non-important things
option(GLFW_BUILD_DOCS ON)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS ON)

## ~ FETCH DEPENDENCIES ~
# Include FetchContent
include(FetchContent)

# Fetch JUCE
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG ${JUCE_VERSION}
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(JUCE)

# Fetch GLFW
FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/archive/refs/tags/${GLFW_VERSION}.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(glfw)

# Fetch GLM
FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/archive/refs/tags/${GLM_VERSION}.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(glm)

# Fetch GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG c
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Populate(glad)

# Include GLAD
include_directories(${glad_SOURCE_DIR}/include)

## ~ COMPILER SETTINGS ~
# Set compiler flags based on compiler
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") # Check if using GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
    # -Wall -Wextra -Wpedantic
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") # Check if using Clang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

## ~ BUILD FILES ~
# Set which project you would like to build
set(B_TARGET "src")

# Set source files
file(GLOB VENDORS_SOURCES ${glad_SOURCE_DIR}/src/glad.c)
file(GLOB_RECURSE PROJECT_HEADERS ${B_TARGET}/*.h)
file(GLOB_RECURSE PROJECT_SOURCES ${B_TARGET}/*.cpp)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

# Add globs to sources
source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

# Important GLFW definitions
add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

## ~ BUILD PROJECTS ~

# Option 1: Build your existing OpenGL/GLFW ray marcher
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})

# Include libraries for ray marcher
target_link_libraries(${PROJECT_NAME} glfw glm)

# Option 2: Create a JUCE audio app (separate target)
# juce_add_gui_app(Ray_Marcher_Audio
#     PRODUCT_NAME "Ray Marcher Audio"
#     COMPANY_NAME "YourCompany"
#     VERSION "1.0.0"
# )
#
# # Configure JUCE modules for audio app
# target_compile_definitions(Ray_Marcher_Audio
#     PRIVATE
#         JUCE_WEB_BROWSER=0
#         JUCE_USE_CURL=0
#         JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:Ray_Marcher_Audio,JUCE_PRODUCT_NAME>"
#         JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:Ray_Marcher_Audio,JUCE_VERSION>"
# )
#
# # Link JUCE modules to audio app
# target_link_libraries(Ray_Marcher_Audio
#     PRIVATE
#         juce::juce_gui_extra
#         juce::juce_audio_utils
#         juce::juce_opengl  # For OpenGL integration with JUCE
#     PUBLIC
#         juce::juce_recommended_config_flags
#         juce::juce_recommended_lto_flags
#         juce::juce_recommended_warning_flags
# )

# Option 3: Hybrid approach - integrate JUCE into your existing app
# Uncomment these lines if you want to add JUCE to your main executable instead
target_link_libraries(${PROJECT_NAME} 
    glfw 
    glm
    # juce::juce_audio_utils
)
    # juce::juce_opengl
    # juce::juce_gui_extra

# target_compile_definitions(${PROJECT_NAME}
#     PRIVATE
#         JUCE_WEB_BROWSER=0
#         JUCE_USE_CURL=0
# )
