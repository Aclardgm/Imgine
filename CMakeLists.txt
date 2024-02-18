cmake_minimum_required(VERSION 3.4)

###Project Configuration 
project(Imgine VERSION 0.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_GENERATOR_PLATFORM x64)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${PROJECT_SOURCE_DIR}/libs CACHE PATH "Missing description." FORCE)

### Fetch GLFW
FetchContent_Declare(
	glfw
	GIT_REPOSITORY	https://github.com/glfw/glfw.git 
	GIT_TAG 	3.3.9  
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



### Fetch GLM
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	1.0.0  #refs/tags/0.9.9.8
)

FetchContent_MakeAvailable(glm)


### Fetch ASSIMP
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



### Fetch FLECS (Entity Component System)
FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG v3.2.11 
)
FetchContent_MakeAvailable(flecs)



### Fetch ImGUI (UI Lib) 
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.3 
)
FetchContent_MakeAvailable(imgui)


##FetchContent_GetProperties(imgui)
# if (NOT imgui_POPULATED)
    # FetchContent_Populate(imgui)
    # set(IMGUI_INCLUDE_DIR ${imgui_SOURCE_DIR}/)
    # file(GLOB IMGUI_SOURCES ${imgui_SOURCE_DIR}/*.cpp)
    # file(GLOB IMGUI_HEADERS ${imgui_SOURCE_DIR}/*.h)
    # add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_SOURCES} ${imgui_SOURCE_DIR}/examples/imgui_impl_glfw.cpp ${imgui_SOURCE_DIR}/examples/imgui_impl_vulkan.cpp)
    # add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
     # target_include_directories(imgui PUBLIC ${IMGUI_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${GLAD_INCLUDE_DIR})
    # target_link_libraries(imgui ${OPENGL_LIBRARIES} glfw glad)
# endif ()

set(IMGUI_SOURCE_FILES 
                ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp 
                ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp 
                ${imgui_SOURCE_DIR}/imgui.cpp 
                ${imgui_SOURCE_DIR}/imgui_draw.cpp 
                ${imgui_SOURCE_DIR}/imgui_demo.cpp 
                ${imgui_SOURCE_DIR}/imgui_tables.cpp 
                ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)


### Fetch STB (Image write and Load)
# FetchContent_Declare(
#     stb
#     GIT_REPOSITORY https://github.com/nothings/stb.git
#     GIT_TAG v1.90.3 
# )
# FetchContent_MakeAvailable(stb)





### Fetch Boost
# set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
# set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
# set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
# set(Boost_USE_MULTITHREADED      ON)
# set(Boost_USE_STATIC_RUNTIME    OFF)
# find_package(Boost 1.84.0)


### Fetch Boost
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)


find_package(Vulkan REQUIRED)


### Main App
set(SOURCE_FILES src/main.cpp )

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
    include/stb
	include/
)



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

set(LIBS
${BOOST_LIBS}
glfw 
${GLFW_LIBRARIES}
glm 
assimp 
flecs::flecs_static 
Vulkan::Vulkan 
Catch2::Catch2WithMain)


### Links Main App
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glm assimp flecs::flecs_static Vulkan::Vulkan Catch2::Catch2WithMain)




### Tests 
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





# ### Dowload Submodules
# find_package(Git QUIET)
# if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# #Update submodules as needed
#     option(GIT_SUBMODULE "Check submodules during build" ON)
#     if(GIT_SUBMODULE)
#         message(STATUS "Submodule update")
#         execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
#                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#                         RESULT_VARIABLE GIT_SUBMOD_RESULT)
#         if(NOT GIT_SUBMOD_RESULT EQUAL "0")
#             message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
#         endif()
#     endif()
# endif()


# # CHECK ALL SUBMODULES
# if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/glfw/CMakeLists.txt")
#     message(FATAL_ERROR "glfw submodules was not dowloaded ! GIT_SUBMODULE was turned off or failed")
# endif()