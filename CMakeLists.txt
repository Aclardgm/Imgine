cmake_minimum_required(VERSION 3.29)

### Project Configuration 
project(Imgine VERSION 0.0.0 LANGUAGES CXX)

## Tell visual studio to use Imgine as startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Imgine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_GENERATOR_PLATFORM x64)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${PROJECT_SOURCE_DIR}/libs CACHE PATH "Missing description." FORCE)

### Fetch GLFW ########################################################################################
FetchContent_Declare(
	glfw
	GIT_REPOSITORY	https://github.com/glfw/glfw.git 
	GIT_TAG 	3.3-stable
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
    set(GLFW_DOCUMENT_INTERNALS OFF CACHE INTERNAL "Include internals in documentation")
    
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()



### Fetch GLM ########################################################################################
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	1.0.0  #refs/tags/0.9.9.8
)

FetchContent_MakeAvailable(glm)


### Fetch ASSIMP ########################################################################################
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.3.1 
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)



### Fetch FLECS (Entity Component System) ########################################################################################
FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG v3.2.11 
)
FetchContent_MakeAvailable(flecs)



### Fetch ImGUI (UI Lib)  ########################################################################################
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.3 
)
FetchContent_MakeAvailable(imgui)


set(IMGUI_SOURCE_FILES 
                ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp 
                ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp 
                ${imgui_SOURCE_DIR}/imgui.cpp 
                ${imgui_SOURCE_DIR}/imgui_draw.cpp 
                ${imgui_SOURCE_DIR}/imgui_demo.cpp 
                ${imgui_SOURCE_DIR}/imgui_tables.cpp 
                ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)


### Fetch Vulkan Memory Allocator  ########################################################################################
FetchContent_Declare(
    vma
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG v3.0.1
)

FetchContent_GetProperties(vma)
if(NOT vma_POPULATED)
  FetchContent_Populate(vma)
endif()
FetchContent_MakeAvailable(vma)









### Fetch Boost ########################################################################################
# set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
# set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
# set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
# set(Boost_USE_MULTITHREADED      ON)
# set(Boost_USE_STATIC_RUNTIME    OFF)
# find_package(Boost 1.84.0)
set(BOOST_LIBS 
Boost::boost
Boost::headers 
Boost::atomic 
Boost::chrono 
Boost::container 
Boost::date_time 
Boost::exception 
Boost::regex 
Boost::system 
Boost::thread)


Include(FetchContent)

### Fetch Catch2 ########################################################################################
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)


find_package(Vulkan REQUIRED)


### Main App ########################################################################################

file(GLOB SOURCE_FILES
		"src/*.cpp"
		"include/*.h")


add_executable(${PROJECT_NAME} 
                ${IMGUI_SOURCE_FILES}
                ${SOURCE_FILES}
)



target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    libs/glfw-src/include
    libs/glm-src
    libs/assimp-src/include
    libs/imgui-src
    libs/imgui-src/backends
    libs/flecs-src
	boost/boost_1_82_0
	${vma_SOURCE_DIR}/include
    include/stb
	include/
)




set(LIBS
${BOOST_LIBS}
glfw 
${GLFW_LIBRARIES}
glm 
assimp 
flecs::flecs_static 
Vulkan::Vulkan 
Catch2::Catch2WithMain
vma)


### Links Main App ########################################################################################
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glm assimp flecs::flecs_static Vulkan::Vulkan Catch2::Catch2WithMain)


### Copy /textures/ directory to target ##########################################################

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/textures"
        "${CMAKE_BINARY_DIR}/Debug/textures"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/textures"
        "${CMAKE_BINARY_DIR}/Release/textures"
)



add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/textures"
        "${CMAKE_BINARY_DIR}/textures"
)


### Copy /models/ directory to target ##########################################################

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/models"
        "${CMAKE_BINARY_DIR}/Debug/models"
)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/models"
        "${CMAKE_BINARY_DIR}/Release/models"
)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/models"
        "${CMAKE_BINARY_DIR}/models"
)


### Tests  ########################################################################################
set(TEST_PROJECT_NAME "TESTS_${PROJECT_NAME}")
set(TESTS_SOURCE_FILES src/tests/imgine_jobysystem_test.cpp)
add_executable(${TEST_PROJECT_NAME} 
                ${IMGUI_SOURCE_FILES}
                ${TESTS_SOURCE_FILES}
)
target_include_directories(
    ${TEST_PROJECT_NAME}
    PUBLIC
    libs/glfw-src/include
    libs/glm-src
    libs/assimp-src/include
    libs/imgui-src
    libs/imgui-src/backends
    libs/flecs-src
	boost/boost_1_82_0
    include/stb
	include/
)

target_link_libraries(${TEST_PROJECT_NAME} glfw ${GLFW_LIBRARIES})
target_link_libraries(${TEST_PROJECT_NAME} glm assimp flecs::flecs_static Vulkan::Vulkan Catch2::Catch2WithMain)


## Compile shaders #######################################################################


###if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
###else()
###  set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslc.exe")
###endif()


set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "shaders/*.frag"
    "shaders/*.vert"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV} -C -v
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(${TEST_PROJECT_NAME} Shaders)

add_custom_command(TARGET ${TEST_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TEST_PROJECT_NAME}>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:${TEST_PROJECT_NAME}>/shaders"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
)

