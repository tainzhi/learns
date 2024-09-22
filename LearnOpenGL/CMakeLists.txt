cmake_minimum_required(VERSION 3.12)

# enable C++17
set(CMAKE_CXX_STANDARD 17)

project (myLearnOpenGL)

if(CMAKE_SYSTEM_NAME MATCHES "Linux") 
        set(CURRENT_SYSTEM "Linux")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(CURRENT_SYSTEM "Windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD") 
        set(CURRENT_SYSTEM "Apple")
        message(STATUS "current platform: Apple OS(FreeBSD)")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS} -I/usr/local/lib -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -lglfw")
endif()


# 从根目录下Textures加载图片
set(RESOURCE_ROOT "${CMAKE_SOURCE_DIR}/resources/")
message("source_root=${RESOURCE_ROOT}")
# 每个课程对应地当前目录
set(SRC_ROOT "${CMAKE_SOURCE_DIR}/1-Lighting/Colors")
message("src_root=${SRC_ROOT}")
file(GLOB MAIN_SRC "${SRC_ROOT}/main.cc")
message("main_src=${MAIN_SRC}")

# 在根目录下configuration定义，然后 build/configuration/root_directory.h 
configure_file(${CMAKE_SOURCE_DIR}/configuration/root_directory.h.in ${CMAKE_BINARY_DIR}/configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

# glad
file(GLOB GLAD_SRC "${CMAKE_SOURCE_DIR}/glad/src/glad.c")
file(GLOB GLAD_HEADER "${CMAKE_SOURCE_DIR}/glad/include/")

# glfw3
if(CMAKE_CXX_COMPILER MATCHES ".*mingw.*")  
        message(STATUS "MinGW compiler detected, so link glfw3 mingw library")  
        set(GLFW3_LIB "${CMAKE_SOURCE_DIR}/glfw3/lib/mingw/libglfw3.a")
else()  
        if(CMAKE_SYSTEM_NAME MATCHES "Linux") 
                message(STATUS "link glfw3 linux library")  
                set(GLFW3_LIB "${CMAKE_SOURCE_DIR}/glfw3/lib/Linux/libglfw3.a")
        elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
                message(STATUS "link glfw3 windows library")  
                set(GLFW3_LIB "${CMAKE_SOURCE_DIR}/glfw3/lib/Windows/glfw3.lib")
        elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD") 
                message(STATUS "link glfw3 apple library")  
                set(GLFW3_LIB "${CMAKE_SOURCE_DIR}/glfw3/lib/Apple/libglfw3.a")
        endif()
endif()
add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
        IMPORTED_LOCATION ${GLFW3_LIB}
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/glfw3/include"
)

# add_library(soil STATIC IMPORTED)
# set_target_properties(soil PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libSOIL.a)
#添加头文件
include_directories(
        ${CMAKE_SOURCE_DIR}/include
        "${PROJECT_SOURCE_DIR}/glad/include/"
)

# 生成可执行文件 ${PROJECT_NAME}
add_executable(${PROJECT_NAME} ${GLAD_SRC} ${MAIN_SRC})

# 为可执行文件${PROJECT_NAME}添加共享库链接
target_link_libraries(${PROJECT_NAME} 
        glfw3
)