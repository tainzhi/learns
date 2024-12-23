## vscode c++

### ~~set build environment in windows~~
在公司环境下使用vc++有法律风险，建议使用mingw

msvs can be installed without installing Visual Studio IDE. [参考](https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-160)
### debug
在`.vscode/tasks.json`中添加了cmake的参数`-DCMAKE_BUILD_TYPE=Debug`, 确定`label:make_run`.

那么在`.vscode/launch.json`中指定`gdb`位置和`"preLaunchTask": "make_run"`, 显然`make_run`就是`tasks.json`中的任务.

task的执行从Terminal > Run Build Task即可.

`launch.json`从左侧的Run And Debug生成后launch.json并配置好后, 就可以直接F5运行

## build/run
### in mingw
建议在windows下用 mingw，因为vc++在公司环境下使用有法律风险。
通过 scoop 安装cpp环境 `scoop install cmake` 和 `scoop install mingw`
```shell
# 指定mingw 编译
cmake -S. -Bbuild  -G "MinGW Makefiles"
# 构建可执行程序到 build目录下
cmake --build build
# 在 build目录下生成可执行程序
```
#### in linux
```shell
cmake -S. -Bbuild
cmake --build build
# 在 build目录下生成可执行程序
```

## 依赖的第三方库

### download and install [soil](https://github.com/littlstar/soil)
- 不使用soil, 使用[stb_image](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)
- 图片路径只能绝对路径

### [dowaload glm](https://github.com/g-truc/glm)
- a header-only library, and thus does not need to be compiled

### [glfw](https://learnopengl.com/Getting-started/Creating-a-window)
https://www.glfw.org/docs/latest/compile.html
一般不需要重新配置，目前已经编译好 Apple/Linux/windows/mingw 系统的lib库。如果需要，可以自行编译添加

### [assimp](https://github.com/assimp/assimp)
目前已经编译后 mingw/linux 系统的lib库
- mingw: 官方库在Mingw下编译出错，故下载编译好的mingw assip库[msys2 assimp](https://packages.msys2.org/packages/mingw-w64-x86_64-assimp)
- linux: 源码编译

## 遇到的问题
#### windows 10无法找到动态库 dll
When using MinGW tools, the find_library() command no longer finds .dll files by default. Instead, it expects .dll.a import libraries to be available.
[更多参考](https://github.com/msys2/MINGW-packages/issues/6394)


#### macOS build error: [`Undefined symbols for architecture x86_64`](https://github.com/cdcseacave/openMVS/issues/202)
add `set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS} -I/usr/local/lib -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -lglfw")` to CMakeLists.txt
