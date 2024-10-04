cmake_minimum_required(VERSION 3.18)

# enable C++23
set(CMAKE_CXX_STANDARD 23)

project (LearnOpenGL)

if(CMAKE_SYSTEM_NAME MATCHES "Linux") 
        set(CURRENT_SYSTEM "Linux")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(CURRENT_SYSTEM "Windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD") 
        set(CURRENT_SYSTEM "Apple")
        message(STATUS "current platform: Apple OS(FreeBSD)")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS} -I/usr/local/lib -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -lglfw")
endif()

# 在根目录下configuration定义，然后 build/configuration/root_directory.h 
configure_file(${CMAKE_SOURCE_DIR}/configuration/root_directory.h.in ${CMAKE_BINARY_DIR}/configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

# glad
file(GLOB GLAD_SRC "${CMAKE_SOURCE_DIR}/3rd-libs/glad/src/glad.c")
file(GLOB GLAD_HEADER "${CMAKE_SOURCE_DIR}/3rd-libs/glad/include/")

# glfw3
if(CMAKE_CXX_COMPILER MATCHES ".*mingw.*")  
        message(STATUS "MinGW compiler detected, so link glfw3 mingw library")  
        set(GLFW3_LIB "${CMAKE_SOURCE_DIR}/3rd-libs/glfw3/lib/mingw/libglfw3.a")
else()  
        if(CMAKE_SYSTEM_NAME MATCHES "Linux") 
                message(STATUS "link glfw3 linux library")  
                set(GLFW3_LIB "${CMAKE_SOURCE_DIR}/3rd-libs/glfw3/lib/Linux/libglfw3.a")
        elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
                message(STATUS "link glfw3 windows library")  
                set(GLFW3_LIB "${CMAKE_SOURCE_DIR}/3rd-libs/glfw3/lib/Windows/glfw3.lib")
        elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD") 
                message(STATUS "link glfw3 apple library")  
                set(GLFW3_LIB "${CMAKE_SOURCE_DIR}/3rd-libs/glfw3/lib/Apple/libglfw3.a")
        endif()
endif()
add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
        IMPORTED_LOCATION ${GLFW3_LIB}
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/3rd-libs/glfw3/include"
)

# assimp
if(CMAKE_CXX_COMPILER MATCHES ".*mingw.*")  
        message(STATUS "MinGW compiler detected, so link assimp mingw library")  
        # download from msys2 packege: https://packages.msys2.org/packages/mingw-w64-x86_64-assimp
        set(ASSIMP_LIB "${CMAKE_SOURCE_DIR}/3rd-libs/assimp/lib/mingw/libassimp.a")
else()  
        message(FATAL_ERROR "no assimp library for non-mingw compilers, please add one into ${CMAKE_SOURCE_DIR}/3rd-libs/assimp/lib/<platform-name>/<assimp name>.a")
        return()
endif()
add_library(assimp STATIC IMPORTED)
set_target_properties(assimp PROPERTIES
        IMPORTED_LOCATION ${ASSIMP_LIB}
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/3rd-libs/assimp/include"
)


# 从根目录下Textures加载图片
set(RESOURCE_ROOT "${CMAKE_SOURCE_DIR}/resources/")
message("source_root=${RESOURCE_ROOT}")

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        "${PROJECT_SOURCE_DIR}/3rd-libs/glad/include/"
        "${PROJECT_SOURCE_DIR}/3rd-libs/assimp/include/"
)

# todo: uncomment
# set(1_PROJECT "1.Gettings_Started")
# # Getting Started - camera - mouse
# file(GLOB 1_PROJECT_SRC "${CMAKE_SOURCE_DIR}/src/${1_PROJECT}/camera/mouse/main.cc")
# add_executable(${1_PROJECT} ${GLAD_SRC} ${1_PROJECT_SRC})
# # 为可执行文件${PROJECT_NAME}添加共享库链接
# target_link_libraries(${1_PROJECT} 
#         glfw3
# )

# todo: uncomment
# set(2_PROJECT "2.Lighting")
# file(GLOB 2_PROJECT_SRC "${PROJECT_SOURCE_DIR}/src/${2_PROJECT}/Colors/*.cc")
# add_executable(${2_PROJECT} ${GLAD_SRC} ${2_PROJECT_SRC})
# target_link_libraries(${2_PROJECT} glfw3)

# todo: uncomment
# set(3_PROJECT "3.Model_Loading")
# file(GLOB 3_PROJECT_SRC "${PROJECT_SOURCE_DIR}/src/${3_PROJECT}/*.cc")
# message(INFO "3_PROJECT_SRC=${3_PROJECT_SRC}")
# add_executable(${3_PROJECT} ${GLAD_SRC} ${3_PROJECT_SRC})
# target_link_libraries(${3_PROJECT} glfw3 assimp)

set(4_PROJECT "4.Advanced_OpenGL")
file(GLOB 4_PROJECT_SRC "${PROJECT_SOURCE_DIR}/src/${4_PROJECT}/FrameBuffers/*.cc")
add_executable(${4_PROJECT} ${GLAD_SRC} ${4_PROJECT_SRC} )
target_link_libraries(${4_PROJECT} glfw3)