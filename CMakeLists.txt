cmake_minimum_required(VERSION 3.13)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")


project(game)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE GAME_SOURCES_C CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_executable("${CMAKE_PROJECT_NAME}" "${GAME_SOURCES}" "${GAME_SOURCES_C}")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC "_DEBUG")
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/") # This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line
else()
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/") # This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line
endif()

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/include/SDL2")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sokol")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/lib")
target_link_libraries("${CMAKE_PROJECT_NAME}" PUBLIC mingw32) # We have to add this before SDL2main so it can find WinMain
target_link_libraries("${CMAKE_PROJECT_NAME}" PUBLIC SDL2main)
target_link_libraries("${CMAKE_PROJECT_NAME}" PUBLIC SDL2)


# if (EMSCRIPTEN)
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 --shell-file ${CMAKE_SOURCE_DIR}/src/shell.html ")
#     set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.

#     set(PLATFORM Web)
#     set(OPENGL_VERSION "ES 2.0")
# else()
#     set(PLATFORM Desktop)
#     set(OPENGL_VERSION 3.3)

# endif ()