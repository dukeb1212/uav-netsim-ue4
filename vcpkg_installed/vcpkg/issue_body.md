Package: zeromq:x64-windows@4.3.5#2

**Host Environment**

- Host: x64-windows
- Compiler: MSVC 19.42.34436.0
-    vcpkg-tool version: 2024-09-30-ab8988503c7cffabfd440b243a383c0a352a023d
    vcpkg-readonly: true
    vcpkg-scripts version: 2960d7d80e8d09c84ae8abf15c12196c2ca7d39a

**To Reproduce**

`vcpkg install `

**Failure logs**

```
-- Downloading https://github.com/zeromq/libzmq/archive/v4.3.5.tar.gz -> zeromq-libzmq-v4.3.5.tar.gz...
-- Extracting source C:/Users/Duke/AppData/Local/vcpkg/downloads/zeromq-libzmq-v4.3.5.tar.gz
-- Applying patch fix-arm.patch
-- Using source at //Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/src/v4.3.5-37c87b6b8c.clean
-- Downloading https://mirror.msys2.org/mingw/mingw64/mingw-w64-x86_64-pkgconf-1~2.2.0-1-any.pkg.tar.zst;https://repo.msys2.org/mingw/mingw64/mingw-w64-x86_64-pkgconf-1~2.2.0-1-any.pkg.tar.zst;https://mirror.yandex.ru/mirrors/msys2/mingw/mingw64/mingw-w64-x86_64-pkgconf-1~2.2.0-1-any.pkg.tar.zst;https://mirrors.tuna.tsinghua.edu.cn/msys2/mingw/mingw64/mingw-w64-x86_64-pkgconf-1~2.2.0-1-any.pkg.tar.zst;https://mirrors.ustc.edu.cn/msys2/mingw/mingw64/mingw-w64-x86_64-pkgconf-1~2.2.0-1-any.pkg.tar.zst;https://mirror.selfnet.de/msys2/mingw/mingw64/mingw-w64-x86_64-pkgconf-1~2.2.0-1-any.pkg.tar.zst -> msys2-mingw-w64-x86_64-pkgconf-1~2.2.0-1-any.pkg.tar.zst...
-- Downloading https://mirror.msys2.org/msys/x86_64/msys2-runtime-3.5.3-3-x86_64.pkg.tar.zst;https://repo.msys2.org/msys/x86_64/msys2-runtime-3.5.3-3-x86_64.pkg.tar.zst;https://mirror.yandex.ru/mirrors/msys2/msys/x86_64/msys2-runtime-3.5.3-3-x86_64.pkg.tar.zst;https://mirrors.tuna.tsinghua.edu.cn/msys2/msys/x86_64/msys2-runtime-3.5.3-3-x86_64.pkg.tar.zst;https://mirrors.ustc.edu.cn/msys2/msys/x86_64/msys2-runtime-3.5.3-3-x86_64.pkg.tar.zst;https://mirror.selfnet.de/msys2/msys/x86_64/msys2-runtime-3.5.3-3-x86_64.pkg.tar.zst -> msys2-msys2-runtime-3.5.3-3-x86_64.pkg.tar.zst...
-- Using msys root at C:/Users/Duke/AppData/Local/vcpkg/downloads/tools/msys2/1e74ca60daa10104
-- Found external ninja('1.12.1').
-- Configuring x64-windows
CMake Error at scripts/cmake/vcpkg_execute_required_process.cmake:127 (message):
    Command failed: "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" -v
    Working Directory: //Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel/vcpkg-parallel-configure
    Error code: 1
    See logs for more information:
      \\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\config-x64-windows-dbg-CMakeCache.txt.log
      \\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\config-x64-windows-rel-CMakeCache.txt.log
      \\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\config-x64-windows-dbg-CMakeConfigureLog.yaml.log
      \\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\config-x64-windows-rel-CMakeConfigureLog.yaml.log
      \\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\config-x64-windows-out.log

Call Stack (most recent call first):
  //Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/x64-windows/share/vcpkg-cmake/vcpkg_cmake_configure.cmake:269 (vcpkg_execute_required_process)
  C:/Users/Duke/AppData/Local/vcpkg/registries/git-trees/454f5e2648cea10a305d0e781fd3093073153a2e/portfile.cmake:30 (vcpkg_cmake_configure)
  scripts/ports.cmake:192 (include)



```

<details><summary>\\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\config-x64-windows-dbg-CMakeConfigureLog.yaml.log</summary>

```

---
events:
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineSystem.cmake:205 (message)"
      - "CMakeLists.txt:2 (project)"
    message: |
      The system is: Windows - 10.0.26100 - AMD64
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:17 (message)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:64 (__determine_compiler_id_test)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCCompiler.cmake:123 (CMAKE_DETERMINE_COMPILER_ID)"
      - "CMakeLists.txt:2 (project)"
    message: |
      Compiling the C compiler identification source file "CMakeCCompilerId.c" succeeded.
      Compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe 
      Build flags: /nologo;/DWIN32;/D_WINDOWS;/utf-8;/MP
      Id flags:  
      
      The output was:
      0
      CMakeCCompilerId.c
      
      
      Compilation of the C compiler identification source "CMakeCCompilerId.c" produced "CMakeCCompilerId.exe"
      
      Compilation of the C compiler identification source "CMakeCCompilerId.c" produced "CMakeCCompilerId.obj"
      
      The C compiler identification is MSVC, found in:
        //Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-dbg/CMakeFiles/3.29.5-msvc4/CompilerIdC/CMakeCCompilerId.exe
      
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:1216 (message)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:250 (CMAKE_DETERMINE_MSVC_SHOWINCLUDES_PREFIX)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCCompiler.cmake:123 (CMAKE_DETERMINE_COMPILER_ID)"
      - "CMakeLists.txt:2 (project)"
    message: |
      Detecting C compiler /showIncludes prefix:
        main.c
        Note: including file: \\\\Desktop-8n7nb0d\\d\\UnrealProjects\\C7_building_UE4\\vcpkg_installed\\vcpkg\\blds\\zeromq\\x64-windows-dbg\\CMakeFiles\\ShowIncludes\\foo.h
        
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:1216 (message)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:250 (CMAKE_DETERMINE_MSVC_SHOWINCLUDES_PREFIX)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCCompiler.cmake:123 (CMAKE_DETERMINE_COMPILER_ID)"
      - "CMakeLists.txt:2 (project)"
    message: |
      Detecting C compiler /showIncludes prefix:
        main.c
        Note: including file: \\\\Desktop-8n7nb0d\\d\\UnrealProjects\\C7_building_UE4\\vcpkg_installed\\vcpkg\\blds\\zeromq\\x64-windows-dbg\\CMakeFiles\\ShowIncludes\\foo.h
        
  -
    kind: "message-v1"
    backtrace:
...
Skipped 72 lines
...
      VCPKG_LINKER_FLAGS_RELEASE: ""
      VCPKG_PLATFORM_TOOLSET: "v143"
      VCPKG_PREFER_SYSTEM_LIBS: "OFF"
      VCPKG_SET_CHARSET_FLAG: "ON"
      VCPKG_TARGET_ARCHITECTURE: "x64"
      VCPKG_TARGET_TRIPLET: "x64-windows"
      Z_VCPKG_ROOT_DIR: "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg"
    buildResult:
      variable: "CMAKE_C_ABI_COMPILED"
      cached: true
      stdout: |
        Change Dir: '//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-f8wpj2'
        
        Run Build Command(s): "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" -v cmTC_93377
        [1/2] C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\cl.exe  /nologo   /nologo /DWIN32 /D_WINDOWS /utf-8 /MP   /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 /showIncludes /FoCMakeFiles\\cmTC_93377.dir\\CMakeCCompilerABI.c.obj /FdCMakeFiles\\cmTC_93377.dir\\ /FS -c "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\share\\cmake-3.29\\Modules\\CMakeCCompilerABI.c"
        [2/2] C:\\WINDOWS\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\\cmTC_93377.dir --rc=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\rc.exe --mt=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\mt.exe --manifests  -- C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_93377.dir\\CMakeCCompilerABI.c.obj  /out:cmTC_93377.exe /implib:cmTC_93377.lib /pdb:cmTC_93377.pdb /version:0.0 /machine:x64  /nologo    /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        FAILED: cmTC_93377.exe 
        C:\\WINDOWS\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\\cmTC_93377.dir --rc=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\rc.exe --mt=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\mt.exe --manifests  -- C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_93377.dir\\CMakeCCompilerABI.c.obj  /out:cmTC_93377.exe /implib:cmTC_93377.lib /pdb:cmTC_93377.pdb /version:0.0 /machine:x64  /nologo    /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        '\\\\Desktop-8n7nb0d\\d\\UnrealProjects\\C7_building_UE4\\vcpkg_installed\\vcpkg\\blds\\zeromq\\x64-windows-dbg\\CMakeFiles\\CMakeScratch\\TryCompile-f8wpj2'
        CMD.EXE was started with the above path as the current directory.
        UNC paths are not supported.  Defaulting to Windows directory.\x0d
        ninja: build stopped: subcommand failed.
        
      exitCode: 1
  -
    kind: "try_compile-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeTestCCompiler.cmake:56 (try_compile)"
      - "CMakeLists.txt:2 (project)"
    checks:
      - "Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe"
    directories:
      source: "//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-i1e16e"
      binary: "//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-i1e16e"
    cmakeVariables:
      CMAKE_C_FLAGS: " /nologo /DWIN32 /D_WINDOWS /utf-8 /MP "
      CMAKE_MSVC_DEBUG_INFORMATION_FORMAT: ""
      CMAKE_MSVC_RUNTIME_LIBRARY: "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<STREQUAL:dynamic,dynamic>:DLL>"
      VCPKG_CHAINLOAD_TOOLCHAIN_FILE: "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/toolchains/windows.cmake"
      VCPKG_CRT_LINKAGE: "dynamic"
      VCPKG_CXX_FLAGS: ""
      VCPKG_CXX_FLAGS_DEBUG: ""
      VCPKG_CXX_FLAGS_RELEASE: ""
      VCPKG_C_FLAGS: ""
      VCPKG_C_FLAGS_DEBUG: ""
      VCPKG_C_FLAGS_RELEASE: ""
      VCPKG_INSTALLED_DIR: "//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed"
      VCPKG_LINKER_FLAGS: ""
      VCPKG_LINKER_FLAGS_DEBUG: ""
      VCPKG_LINKER_FLAGS_RELEASE: ""
      VCPKG_PLATFORM_TOOLSET: "v143"
      VCPKG_PREFER_SYSTEM_LIBS: "OFF"
      VCPKG_SET_CHARSET_FLAG: "ON"
      VCPKG_TARGET_ARCHITECTURE: "x64"
      VCPKG_TARGET_TRIPLET: "x64-windows"
      Z_VCPKG_ROOT_DIR: "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg"
    buildResult:
      variable: "CMAKE_C_COMPILER_WORKS"
      cached: true
      stdout: |
        Change Dir: '//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-i1e16e'
        
        Run Build Command(s): "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" -v cmTC_ca806
        [1/2] C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\cl.exe  /nologo   /nologo /DWIN32 /D_WINDOWS /utf-8 /MP   /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 /showIncludes /FoCMakeFiles\\cmTC_ca806.dir\\testCCompiler.c.obj /FdCMakeFiles\\cmTC_ca806.dir\\ /FS -c \\\\Desktop-8n7nb0d\\d\\UnrealProjects\\C7_building_UE4\\vcpkg_installed\\vcpkg\\blds\\zeromq\\x64-windows-dbg\\CMakeFiles\\CMakeScratch\\TryCompile-i1e16e\\testCCompiler.c
        [2/2] C:\\WINDOWS\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\\cmTC_ca806.dir --rc=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\rc.exe --mt=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\mt.exe --manifests  -- C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_ca806.dir\\testCCompiler.c.obj  /out:cmTC_ca806.exe /implib:cmTC_ca806.lib /pdb:cmTC_ca806.pdb /version:0.0 /machine:x64  /nologo    /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        FAILED: cmTC_ca806.exe 
        C:\\WINDOWS\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\\cmTC_ca806.dir --rc=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\rc.exe --mt=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\mt.exe --manifests  -- C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_ca806.dir\\testCCompiler.c.obj  /out:cmTC_ca806.exe /implib:cmTC_ca806.lib /pdb:cmTC_ca806.pdb /version:0.0 /machine:x64  /nologo    /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        '\\\\Desktop-8n7nb0d\\d\\UnrealProjects\\C7_building_UE4\\vcpkg_installed\\vcpkg\\blds\\zeromq\\x64-windows-dbg\\CMakeFiles\\CMakeScratch\\TryCompile-i1e16e'
        CMD.EXE was started with the above path as the current directory.
        UNC paths are not supported.  Defaulting to Windows directory.\x0d
        ninja: build stopped: subcommand failed.
        
      exitCode: 1
...
```
</details>

<details><summary>\\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\config-x64-windows-rel-CMakeConfigureLog.yaml.log</summary>

```

---
events:
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineSystem.cmake:205 (message)"
      - "CMakeLists.txt:2 (project)"
    message: |
      The system is: Windows - 10.0.26100 - AMD64
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:17 (message)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:64 (__determine_compiler_id_test)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCCompiler.cmake:123 (CMAKE_DETERMINE_COMPILER_ID)"
      - "CMakeLists.txt:2 (project)"
    message: |
      Compiling the C compiler identification source file "CMakeCCompilerId.c" succeeded.
      Compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe 
      Build flags: /nologo;/DWIN32;/D_WINDOWS;/utf-8;/MP
      Id flags:  
      
      The output was:
      0
      CMakeCCompilerId.c
      
      
      Compilation of the C compiler identification source "CMakeCCompilerId.c" produced "CMakeCCompilerId.exe"
      
      Compilation of the C compiler identification source "CMakeCCompilerId.c" produced "CMakeCCompilerId.obj"
      
      The C compiler identification is MSVC, found in:
        //Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel/CMakeFiles/3.29.5-msvc4/CompilerIdC/CMakeCCompilerId.exe
      
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:1216 (message)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:250 (CMAKE_DETERMINE_MSVC_SHOWINCLUDES_PREFIX)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCCompiler.cmake:123 (CMAKE_DETERMINE_COMPILER_ID)"
      - "CMakeLists.txt:2 (project)"
    message: |
      Detecting C compiler /showIncludes prefix:
        main.c
        Note: including file: \\\\Desktop-8n7nb0d\\d\\UnrealProjects\\C7_building_UE4\\vcpkg_installed\\vcpkg\\blds\\zeromq\\x64-windows-rel\\CMakeFiles\\ShowIncludes\\foo.h
        
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:1216 (message)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:250 (CMAKE_DETERMINE_MSVC_SHOWINCLUDES_PREFIX)"
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCCompiler.cmake:123 (CMAKE_DETERMINE_COMPILER_ID)"
      - "CMakeLists.txt:2 (project)"
    message: |
      Detecting C compiler /showIncludes prefix:
        main.c
        Note: including file: \\\\Desktop-8n7nb0d\\d\\UnrealProjects\\C7_building_UE4\\vcpkg_installed\\vcpkg\\blds\\zeromq\\x64-windows-rel\\CMakeFiles\\ShowIncludes\\foo.h
        
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeDetermineCompilerId.cmake:17 (message)"
...
Skipped 69 lines
...
      VCPKG_LINKER_FLAGS: ""
      VCPKG_LINKER_FLAGS_DEBUG: ""
      VCPKG_LINKER_FLAGS_RELEASE: ""
      VCPKG_PLATFORM_TOOLSET: "v143"
      VCPKG_PREFER_SYSTEM_LIBS: "OFF"
      VCPKG_SET_CHARSET_FLAG: "ON"
      VCPKG_TARGET_ARCHITECTURE: "x64"
      VCPKG_TARGET_TRIPLET: "x64-windows"
      Z_VCPKG_ROOT_DIR: "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg"
    buildResult:
      variable: "CMAKE_C_ABI_COMPILED"
      cached: true
      stdout: |
        Change Dir: '//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-nqtshs'
        
        Run Build Command(s): "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" -v cmTC_ec3c5
        [1/2] C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\cl.exe  /nologo   /nologo /DWIN32 /D_WINDOWS /utf-8 /MP   /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 /showIncludes /FoCMakeFiles\\cmTC_ec3c5.dir\\CMakeCCompilerABI.c.obj /FdCMakeFiles\\cmTC_ec3c5.dir\\ /FS -c "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\share\\cmake-3.29\\Modules\\CMakeCCompilerABI.c"
        [2/2] C:\\WINDOWS\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\\cmTC_ec3c5.dir --rc=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\rc.exe --mt=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\mt.exe --manifests  -- C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_ec3c5.dir\\CMakeCCompilerABI.c.obj  /out:cmTC_ec3c5.exe /implib:cmTC_ec3c5.lib /pdb:cmTC_ec3c5.pdb /version:0.0 /machine:x64  /nologo    /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        FAILED: cmTC_ec3c5.exe 
        C:\\WINDOWS\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\\cmTC_ec3c5.dir --rc=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\rc.exe --mt=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\mt.exe --manifests  -- C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_ec3c5.dir\\CMakeCCompilerABI.c.obj  /out:cmTC_ec3c5.exe /implib:cmTC_ec3c5.lib /pdb:cmTC_ec3c5.pdb /version:0.0 /machine:x64  /nologo    /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        '\\\\Desktop-8n7nb0d\\d\\UnrealProjects\\C7_building_UE4\\vcpkg_installed\\vcpkg\\blds\\zeromq\\x64-windows-rel\\CMakeFiles\\CMakeScratch\\TryCompile-nqtshs'
        CMD.EXE was started with the above path as the current directory.
        UNC paths are not supported.  Defaulting to Windows directory.\x0d
        ninja: build stopped: subcommand failed.
        
      exitCode: 1
  -
    kind: "try_compile-v1"
    backtrace:
      - "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeTestCCompiler.cmake:56 (try_compile)"
      - "CMakeLists.txt:2 (project)"
    checks:
      - "Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe"
    directories:
      source: "//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-hbncxe"
      binary: "//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-hbncxe"
    cmakeVariables:
      CMAKE_C_FLAGS: " /nologo /DWIN32 /D_WINDOWS /utf-8 /MP "
      CMAKE_MSVC_DEBUG_INFORMATION_FORMAT: ""
      CMAKE_MSVC_RUNTIME_LIBRARY: "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<STREQUAL:dynamic,dynamic>:DLL>"
      VCPKG_CHAINLOAD_TOOLCHAIN_FILE: "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/toolchains/windows.cmake"
      VCPKG_CRT_LINKAGE: "dynamic"
      VCPKG_CXX_FLAGS: ""
      VCPKG_CXX_FLAGS_DEBUG: ""
      VCPKG_CXX_FLAGS_RELEASE: ""
      VCPKG_C_FLAGS: ""
      VCPKG_C_FLAGS_DEBUG: ""
      VCPKG_C_FLAGS_RELEASE: ""
      VCPKG_INSTALLED_DIR: "//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed"
      VCPKG_LINKER_FLAGS: ""
      VCPKG_LINKER_FLAGS_DEBUG: ""
      VCPKG_LINKER_FLAGS_RELEASE: ""
      VCPKG_PLATFORM_TOOLSET: "v143"
      VCPKG_PREFER_SYSTEM_LIBS: "OFF"
      VCPKG_SET_CHARSET_FLAG: "ON"
      VCPKG_TARGET_ARCHITECTURE: "x64"
      VCPKG_TARGET_TRIPLET: "x64-windows"
      Z_VCPKG_ROOT_DIR: "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg"
    buildResult:
      variable: "CMAKE_C_COMPILER_WORKS"
      cached: true
      stdout: |
        Change Dir: '//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-hbncxe'
        
        Run Build Command(s): "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" -v cmTC_fc27c
        [1/2] C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\cl.exe  /nologo   /nologo /DWIN32 /D_WINDOWS /utf-8 /MP   /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 /showIncludes /FoCMakeFiles\\cmTC_fc27c.dir\\testCCompiler.c.obj /FdCMakeFiles\\cmTC_fc27c.dir\\ /FS -c \\\\Desktop-8n7nb0d\\d\\UnrealProjects\\C7_building_UE4\\vcpkg_installed\\vcpkg\\blds\\zeromq\\x64-windows-rel\\CMakeFiles\\CMakeScratch\\TryCompile-hbncxe\\testCCompiler.c
        [2/2] C:\\WINDOWS\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\\cmTC_fc27c.dir --rc=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\rc.exe --mt=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\mt.exe --manifests  -- C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_fc27c.dir\\testCCompiler.c.obj  /out:cmTC_fc27c.exe /implib:cmTC_fc27c.lib /pdb:cmTC_fc27c.pdb /version:0.0 /machine:x64  /nologo    /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        FAILED: cmTC_fc27c.exe 
        C:\\WINDOWS\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\\cmTC_fc27c.dir --rc=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\rc.exe --mt=C:\\PROGRA~2\\WI3CF2~1\\10\\bin\\100226~1.0\\x64\\mt.exe --manifests  -- C:\\PROGRA~1\\MIB055~1\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1442~1.344\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_fc27c.dir\\testCCompiler.c.obj  /out:cmTC_fc27c.exe /implib:cmTC_fc27c.lib /pdb:cmTC_fc27c.pdb /version:0.0 /machine:x64  /nologo    /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        '\\\\Desktop-8n7nb0d\\d\\UnrealProjects\\C7_building_UE4\\vcpkg_installed\\vcpkg\\blds\\zeromq\\x64-windows-rel\\CMakeFiles\\CMakeScratch\\TryCompile-hbncxe'
        CMD.EXE was started with the above path as the current directory.
        UNC paths are not supported.  Defaulting to Windows directory.\x0d
        ninja: build stopped: subcommand failed.
        
      exitCode: 1
...
```
</details>

<details><summary>\\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\config-x64-windows-out.log</summary>

```
[1/2] "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" -E chdir "../../x64-windows-dbg" "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" "//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/src/v4.3.5-37c87b6b8c.clean" "-G" "Ninja" "-DCMAKE_BUILD_TYPE=Debug" "-DCMAKE_INSTALL_PREFIX=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/pkgs/zeromq_x64-windows/debug" "-DFETCHCONTENT_FULLY_DISCONNECTED=ON" "-DZMQ_BUILD_TESTS=OFF" "-DBUILD_STATIC=0" "-DBUILD_SHARED=1" "-DWITH_PERF_TOOL=OFF" "-DWITH_DOCS=OFF" "-DWITH_NSS=OFF" "-DWITH_LIBBSD=OFF" "-DCMAKE_REQUIRE_FIND_PACKAGE_GnuTLS=ON" "-DWITH_LIBSODIUM_STATIC=0" "-DWITH_LIBSODIUM=OFF" "-DENABLE_DRAFTS=OFF" "-DENABLE_WS=OFF" "-DWITH_TLS=OFF" "-DENABLE_CURVE=OFF" "-DPKG_CONFIG_EXECUTABLE=C:/Users/Duke/AppData/Local/vcpkg/downloads/tools/msys2/1e74ca60daa10104/mingw64/bin/pkg-config.exe" "-DCMAKE_MAKE_PROGRAM=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" "-DBUILD_SHARED_LIBS=ON" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/toolchains/windows.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" "-DVCPKG_SET_CHARSET_FLAG=ON" "-DVCPKG_PLATFORM_TOOLSET=v143" "-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY=ON" "-DCMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP=TRUE" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DVCPKG_APPLOCAL_DEPS=OFF" "-DCMAKE_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION=ON" "-DVCPKG_CXX_FLAGS=" "-DVCPKG_CXX_FLAGS_RELEASE=" "-DVCPKG_CXX_FLAGS_DEBUG=" "-DVCPKG_C_FLAGS=" "-DVCPKG_C_FLAGS_RELEASE=" "-DVCPKG_C_FLAGS_DEBUG=" "-DVCPKG_CRT_LINKAGE=dynamic" "-DVCPKG_LINKER_FLAGS=" "-DVCPKG_LINKER_FLAGS_RELEASE=" "-DVCPKG_LINKER_FLAGS_DEBUG=" "-DVCPKG_TARGET_ARCHITECTURE=x64" "-DCMAKE_INSTALL_LIBDIR:STRING=lib" "-DCMAKE_INSTALL_BINDIR:STRING=bin" "-D_VCPKG_ROOT_DIR=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg" "-D_VCPKG_INSTALLED_DIR=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed" "-DVCPKG_MANIFEST_INSTALL=OFF" "-DCMAKE_PDB_OUTPUT_DIRECTORY=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-dbg"
FAILED: ../../x64-windows-dbg/CMakeCache.txt 
...
Skipped 45 lines
...
FAILED: ../CMakeCache.txt 
"C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" -E chdir ".." "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" "//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/src/v4.3.5-37c87b6b8c.clean" "-G" "Ninja" "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_INSTALL_PREFIX=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/pkgs/zeromq_x64-windows" "-DFETCHCONTENT_FULLY_DISCONNECTED=ON" "-DZMQ_BUILD_TESTS=OFF" "-DBUILD_STATIC=0" "-DBUILD_SHARED=1" "-DWITH_PERF_TOOL=OFF" "-DWITH_DOCS=OFF" "-DWITH_NSS=OFF" "-DWITH_LIBBSD=OFF" "-DCMAKE_REQUIRE_FIND_PACKAGE_GnuTLS=ON" "-DWITH_LIBSODIUM_STATIC=0" "-DWITH_LIBSODIUM=OFF" "-DENABLE_DRAFTS=OFF" "-DENABLE_WS=OFF" "-DWITH_TLS=OFF" "-DENABLE_CURVE=OFF" "-DPKG_CONFIG_EXECUTABLE=C:/Users/Duke/AppData/Local/vcpkg/downloads/tools/msys2/1e74ca60daa10104/mingw64/bin/pkg-config.exe" "-DCMAKE_MAKE_PROGRAM=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" "-DBUILD_SHARED_LIBS=ON" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/toolchains/windows.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" "-DVCPKG_SET_CHARSET_FLAG=ON" "-DVCPKG_PLATFORM_TOOLSET=v143" "-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY=ON" "-DCMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP=TRUE" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DVCPKG_APPLOCAL_DEPS=OFF" "-DCMAKE_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION=ON" "-DVCPKG_CXX_FLAGS=" "-DVCPKG_CXX_FLAGS_RELEASE=" "-DVCPKG_CXX_FLAGS_DEBUG=" "-DVCPKG_C_FLAGS=" "-DVCPKG_C_FLAGS_RELEASE=" "-DVCPKG_C_FLAGS_DEBUG=" "-DVCPKG_CRT_LINKAGE=dynamic" "-DVCPKG_LINKER_FLAGS=" "-DVCPKG_LINKER_FLAGS_RELEASE=" "-DVCPKG_LINKER_FLAGS_DEBUG=" "-DVCPKG_TARGET_ARCHITECTURE=x64" "-DCMAKE_INSTALL_LIBDIR:STRING=lib" "-DCMAKE_INSTALL_BINDIR:STRING=bin" "-D_VCPKG_ROOT_DIR=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg" "-D_VCPKG_INSTALLED_DIR=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed" "-DVCPKG_MANIFEST_INSTALL=OFF"
CMake Warning (dev) at CMakeLists.txt:2 (project):
  cmake_minimum_required() should be called prior to this top-level project()
  call.  Please see the cmake-commands(7) manual for usage documentation of
  both commands.
This warning is for project developers.  Use -Wno-dev to suppress it.

-- The C compiler identification is MSVC 19.42.34436.0
-- The CXX compiler identification is MSVC 19.42.34436.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - failed
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe - broken
CMake Error at C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeTestCCompiler.cmake:67 (message):
  The C compiler

    "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe"

  is not able to compile a simple test program.

  It fails with the following output:

    Change Dir: '//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-hbncxe'
    
    Run Build Command(s): "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" -v cmTC_fc27c
    [1/2] C:\PROGRA~1\MIB055~1\2022\COMMUN~1\VC\Tools\MSVC\1442~1.344\bin\Hostx64\x64\cl.exe  /nologo   /nologo /DWIN32 /D_WINDOWS /utf-8 /MP   /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 /showIncludes /FoCMakeFiles\cmTC_fc27c.dir\testCCompiler.c.obj /FdCMakeFiles\cmTC_fc27c.dir\ /FS -c \\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\x64-windows-rel\CMakeFiles\CMakeScratch\TryCompile-hbncxe\testCCompiler.c
    [2/2] C:\WINDOWS\system32\cmd.exe /C "cd . && "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\cmTC_fc27c.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100226~1.0\x64\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100226~1.0\x64\mt.exe --manifests  -- C:\PROGRA~1\MIB055~1\2022\COMMUN~1\VC\Tools\MSVC\1442~1.344\bin\Hostx64\x64\link.exe /nologo CMakeFiles\cmTC_fc27c.dir\testCCompiler.c.obj  /out:cmTC_fc27c.exe /implib:cmTC_fc27c.lib /pdb:cmTC_fc27c.pdb /version:0.0 /machine:x64  /nologo    /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
    FAILED: cmTC_fc27c.exe 
    C:\WINDOWS\system32\cmd.exe /C "cd . && "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\cmTC_fc27c.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100226~1.0\x64\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100226~1.0\x64\mt.exe --manifests  -- C:\PROGRA~1\MIB055~1\2022\COMMUN~1\VC\Tools\MSVC\1442~1.344\bin\Hostx64\x64\link.exe /nologo CMakeFiles\cmTC_fc27c.dir\testCCompiler.c.obj  /out:cmTC_fc27c.exe /implib:cmTC_fc27c.lib /pdb:cmTC_fc27c.pdb /version:0.0 /machine:x64  /nologo    /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
    '\\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\x64-windows-rel\CMakeFiles\CMakeScratch\TryCompile-hbncxe'
    CMD.EXE was started with the above path as the current directory.
    UNC paths are not supported.  Defaulting to Windows directory.
    ninja: build stopped: subcommand failed.
    
    

  

  CMake will not be able to correctly generate this project.
Call Stack (most recent call first):
  CMakeLists.txt:2 (project)


-- Configuring incomplete, errors occurred!
ninja: build stopped: subcommand failed.
```
</details>

<details><summary>\\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\config-x64-windows-rel-CMakeCache.txt.log</summary>

```
# This is the CMakeCache file.
# For build in directory: //Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel
# It was generated by CMake: C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
# You can edit this file to change values found and used by cmake.
# If you do not want to change any of the values, simply exit the editor.
# If you do want to change a value, simply edit, save, and exit the editor.
# The syntax for the file is as follows:
# KEY:TYPE=VALUE
# KEY is the name of a variable in the cache.
# TYPE is a hint to GUIs for the type of VALUE, DO NOT EDIT TYPE!.
# VALUE is the current value for the KEY.

########################
# EXTERNAL cache entries
########################

//No help, variable specified on the command line.
BUILD_SHARED:UNINITIALIZED=1

//No help, variable specified on the command line.
BUILD_SHARED_LIBS:UNINITIALIZED=ON

//No help, variable specified on the command line.
BUILD_STATIC:UNINITIALIZED=0

//Path to a program.
CMAKE_AR:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/lib.exe

//Choose the type of build, options are: None Debug Release RelWithDebInfo
// MinSizeRel ...
CMAKE_BUILD_TYPE:STRING=Release

CMAKE_CROSSCOMPILING:STRING=OFF

//CXX compiler
CMAKE_CXX_COMPILER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe

CMAKE_CXX_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /utf-8 /GR /EHsc /MP '

CMAKE_CXX_FLAGS_DEBUG:STRING='/D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 '

CMAKE_CXX_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//C compiler
CMAKE_C_COMPILER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe

CMAKE_C_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /utf-8 /MP '

CMAKE_C_FLAGS_DEBUG:STRING='/D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 '

//Flags used by the C compiler during MINSIZEREL builds.
CMAKE_C_FLAGS_MINSIZEREL:STRING=/MD /O1 /Ob1 /DNDEBUG

CMAKE_C_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//Flags used by the C compiler during RELWITHDEBINFO builds.
CMAKE_C_FLAGS_RELWITHDEBINFO:STRING=/MD /Zi /O2 /Ob1 /DNDEBUG

//Libraries linked by default with all C applications.
CMAKE_C_STANDARD_LIBRARIES:STRING=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib

//No help, variable specified on the command line.
CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION:UNINITIALIZED=ON

//Flags used by the linker during all build types.
CMAKE_EXE_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during DEBUG builds.
CMAKE_EXE_LINKER_FLAGS_DEBUG:STRING=/nologo    /debug /INCREMENTAL

//Flags used by the linker during MINSIZEREL builds.
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING='/nologo /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF  '

//Flags used by the linker during RELWITHDEBINFO builds.
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//Enable/Disable output of compile commands during generation.
CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=

//No help, variable specified on the command line.
CMAKE_EXPORT_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY:UNINITIALIZED=ON

//Value Computed by CMake.
CMAKE_FIND_PACKAGE_REDIRECTS_DIR:STATIC=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel/CMakeFiles/pkgRedirects

//No help, variable specified on the command line.
CMAKE_INSTALL_BINDIR:STRING=bin

//No help, variable specified on the command line.
CMAKE_INSTALL_LIBDIR:STRING=lib

//Install path prefix, prepended onto install directories.
CMAKE_INSTALL_PREFIX:PATH=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/pkgs/zeromq_x64-windows

//No help, variable specified on the command line.
CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP:UNINITIALIZED=TRUE

//Path to a program.
CMAKE_LINKER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/link.exe

//make program
CMAKE_MAKE_PROGRAM:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe

//Flags used by the linker during the creation of modules during
// all build types.
CMAKE_MODULE_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during the creation of modules during
// DEBUG builds.
CMAKE_MODULE_LINKER_FLAGS_DEBUG:STRING=/nologo    /debug /INCREMENTAL

//Flags used by the linker during the creation of modules during
// MINSIZEREL builds.
CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

...
Skipped 151 lines
...
VCPKG_LINKER_FLAGS_DEBUG:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_LINKER_FLAGS_RELEASE:UNINITIALIZED=

//The path to the vcpkg manifest directory.
VCPKG_MANIFEST_DIR:PATH=

//Use manifest mode, as opposed to classic mode.
VCPKG_MANIFEST_MODE:BOOL=OFF

//No help, variable specified on the command line.
VCPKG_PLATFORM_TOOLSET:UNINITIALIZED=v143

//Appends the vcpkg paths to CMAKE_PREFIX_PATH, CMAKE_LIBRARY_PATH
// and CMAKE_FIND_ROOT_PATH so that vcpkg libraries/packages are
// found after toolchain/system libraries/packages.
VCPKG_PREFER_SYSTEM_LIBS:BOOL=OFF

//Enable the setup of CMAKE_PROGRAM_PATH to vcpkg paths
VCPKG_SETUP_CMAKE_PROGRAM_PATH:BOOL=ON

//No help, variable specified on the command line.
VCPKG_SET_CHARSET_FLAG:UNINITIALIZED=ON

//No help, variable specified on the command line.
VCPKG_TARGET_ARCHITECTURE:UNINITIALIZED=x64

//Vcpkg target triplet (ex. x86-windows)
VCPKG_TARGET_TRIPLET:STRING=x64-windows

//Trace calls to find_package()
VCPKG_TRACE_FIND_PACKAGE:BOOL=OFF

//Enables messages from the VCPKG toolchain for debugging purposes.
VCPKG_VERBOSE:BOOL=OFF

//No help, variable specified on the command line.
WITH_DOCS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
WITH_LIBBSD:UNINITIALIZED=OFF

//No help, variable specified on the command line.
WITH_LIBSODIUM:UNINITIALIZED=OFF

//No help, variable specified on the command line.
WITH_LIBSODIUM_STATIC:UNINITIALIZED=0

//No help, variable specified on the command line.
WITH_NSS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
WITH_PERF_TOOL:UNINITIALIZED=OFF

//No help, variable specified on the command line.
WITH_TLS:UNINITIALIZED=OFF

//(experimental) Automatically copy dependencies into the install
// target directory for executables. Requires CMake 3.14.
X_VCPKG_APPLOCAL_DEPS_INSTALL:BOOL=OFF

//(experimental) Add USES_TERMINAL to VCPKG_APPLOCAL_DEPS to force
// serialization.
X_VCPKG_APPLOCAL_DEPS_SERIALIZED:BOOL=OFF

//No help, variable specified on the command line.
ZMQ_BUILD_TESTS:UNINITIALIZED=OFF

//Value Computed by CMake
ZeroMQ_BINARY_DIR:STATIC=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel

//Value Computed by CMake
ZeroMQ_IS_TOP_LEVEL:STATIC=ON

//Value Computed by CMake
ZeroMQ_SOURCE_DIR:STATIC=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/src/v4.3.5-37c87b6b8c.clean

//The directory which contains the installed libraries for each
// triplet
_VCPKG_INSTALLED_DIR:PATH=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed

//No help, variable specified on the command line.
_VCPKG_ROOT_DIR:UNINITIALIZED=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg


########################
# INTERNAL cache entries
########################

//ADVANCED property for variable: CMAKE_AR
CMAKE_AR-ADVANCED:INTERNAL=1
//This is the directory where this CMakeCache.txt was created
CMAKE_CACHEFILE_DIR:INTERNAL=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-rel
//Major version of cmake used to create the current loaded cache
CMAKE_CACHE_MAJOR_VERSION:INTERNAL=3
//Minor version of cmake used to create the current loaded cache
CMAKE_CACHE_MINOR_VERSION:INTERNAL=29
//Patch version of cmake used to create the current loaded cache
CMAKE_CACHE_PATCH_VERSION:INTERNAL=5
//Path to CMake executable.
CMAKE_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
//Path to cpack program executable.
CMAKE_CPACK_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cpack.exe
//Path to ctest program executable.
CMAKE_CTEST_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe
//ADVANCED property for variable: CMAKE_CXX_COMPILER
CMAKE_CXX_COMPILER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_COMPILER
CMAKE_C_COMPILER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS
CMAKE_C_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_DEBUG
CMAKE_C_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_MINSIZEREL
CMAKE_C_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELEASE
CMAKE_C_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELWITHDEBINFO
CMAKE_C_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_STANDARD_LIBRARIES
CMAKE_C_STANDARD_LIBRARIES-ADVANCED:INTERNAL=1
//Executable file format
CMAKE_EXECUTABLE_FORMAT:INTERNAL=Unknown
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS
CMAKE_EXE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_DEBUG
CMAKE_EXE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELEASE
CMAKE_EXE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXPORT_COMPILE_COMMANDS
CMAKE_EXPORT_COMPILE_COMMANDS-ADVANCED:INTERNAL=1
//Name of external makefile project generator.
CMAKE_EXTRA_GENERATOR:INTERNAL=
//Name of generator.
CMAKE_GENERATOR:INTERNAL=Ninja
//Generator instance identifier.
CMAKE_GENERATOR_INSTANCE:INTERNAL=
//Name of generator platform.
CMAKE_GENERATOR_PLATFORM:INTERNAL=
//Name of generator toolset.
CMAKE_GENERATOR_TOOLSET:INTERNAL=
//Source directory with the top level CMakeLists.txt file for this
// project
CMAKE_HOME_DIRECTORY:INTERNAL=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/src/v4.3.5-37c87b6b8c.clean
//ADVANCED property for variable: CMAKE_LINKER
CMAKE_LINKER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS
CMAKE_MODULE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_DEBUG
CMAKE_MODULE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL
CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELEASE
CMAKE_MODULE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MT
CMAKE_MT-ADVANCED:INTERNAL=1
//number of local generators
CMAKE_NUMBER_OF_MAKEFILES:INTERNAL=1
//Platform information initialized
CMAKE_PLATFORM_INFO_INITIALIZED:INTERNAL=1
//noop for ranlib
CMAKE_RANLIB:INTERNAL=:
//ADVANCED property for variable: CMAKE_RC_COMPILER
CMAKE_RC_COMPILER-ADVANCED:INTERNAL=1
CMAKE_RC_COMPILER_WORKS:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS
CMAKE_RC_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_DEBUG
CMAKE_RC_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_MINSIZEREL
CMAKE_RC_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELEASE
CMAKE_RC_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELWITHDEBINFO
CMAKE_RC_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//Path to CMake installation.
CMAKE_ROOT:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS
CMAKE_SHARED_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_DEBUG
CMAKE_SHARED_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELEASE
CMAKE_SHARED_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_INSTALL_RPATH
CMAKE_SKIP_INSTALL_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_RPATH
CMAKE_SKIP_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS
CMAKE_STATIC_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_DEBUG
CMAKE_STATIC_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL
CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELEASE
CMAKE_STATIC_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_TOOLCHAIN_FILE
CMAKE_TOOLCHAIN_FILE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_VERBOSE_MAKEFILE
CMAKE_VERBOSE_MAKEFILE-ADVANCED:INTERNAL=1
//Install the dependencies listed in your manifest:
//\n    If this is off, you will have to manually install your dependencies.
//\n    See https://github.com/microsoft/vcpkg/tree/master/docs/specifications/manifests.md
// for more info.
//\n
VCPKG_MANIFEST_INSTALL:INTERNAL=OFF
//ADVANCED property for variable: VCPKG_VERBOSE
VCPKG_VERBOSE-ADVANCED:INTERNAL=1
//Making sure VCPKG_MANIFEST_MODE doesn't change
Z_VCPKG_CHECK_MANIFEST_MODE:INTERNAL=OFF
//Vcpkg root directory
Z_VCPKG_ROOT_DIR:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg

```
</details>

<details><summary>\\Desktop-8n7nb0d\d\UnrealProjects\C7_building_UE4\vcpkg_installed\vcpkg\blds\zeromq\config-x64-windows-dbg-CMakeCache.txt.log</summary>

```
# This is the CMakeCache file.
# For build in directory: //Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-dbg
# It was generated by CMake: C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
# You can edit this file to change values found and used by cmake.
# If you do not want to change any of the values, simply exit the editor.
# If you do want to change a value, simply edit, save, and exit the editor.
# The syntax for the file is as follows:
# KEY:TYPE=VALUE
# KEY is the name of a variable in the cache.
# TYPE is a hint to GUIs for the type of VALUE, DO NOT EDIT TYPE!.
# VALUE is the current value for the KEY.

########################
# EXTERNAL cache entries
########################

//No help, variable specified on the command line.
BUILD_SHARED:UNINITIALIZED=1

//No help, variable specified on the command line.
BUILD_SHARED_LIBS:UNINITIALIZED=ON

//No help, variable specified on the command line.
BUILD_STATIC:UNINITIALIZED=0

//Path to a program.
CMAKE_AR:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/lib.exe

//Choose the type of build, options are: None Debug Release RelWithDebInfo
// MinSizeRel ...
CMAKE_BUILD_TYPE:STRING=Debug

CMAKE_CROSSCOMPILING:STRING=OFF

//CXX compiler
CMAKE_CXX_COMPILER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe

CMAKE_CXX_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /utf-8 /GR /EHsc /MP '

CMAKE_CXX_FLAGS_DEBUG:STRING='/D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 '

CMAKE_CXX_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//C compiler
CMAKE_C_COMPILER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433/bin/Hostx64/x64/cl.exe

CMAKE_C_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /utf-8 /MP '

CMAKE_C_FLAGS_DEBUG:STRING='/D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 '

//Flags used by the C compiler during MINSIZEREL builds.
CMAKE_C_FLAGS_MINSIZEREL:STRING=/MD /O1 /Ob1 /DNDEBUG

CMAKE_C_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//Flags used by the C compiler during RELWITHDEBINFO builds.
CMAKE_C_FLAGS_RELWITHDEBINFO:STRING=/MD /Zi /O2 /Ob1 /DNDEBUG

//Libraries linked by default with all C applications.
CMAKE_C_STANDARD_LIBRARIES:STRING=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib

//No help, variable specified on the command line.
CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION:UNINITIALIZED=ON

//Flags used by the linker during all build types.
CMAKE_EXE_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during DEBUG builds.
CMAKE_EXE_LINKER_FLAGS_DEBUG:STRING=/nologo    /debug /INCREMENTAL

//Flags used by the linker during MINSIZEREL builds.
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING='/nologo /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF  '

//Flags used by the linker during RELWITHDEBINFO builds.
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//Enable/Disable output of compile commands during generation.
CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=

//No help, variable specified on the command line.
CMAKE_EXPORT_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY:UNINITIALIZED=ON

//Value Computed by CMake.
CMAKE_FIND_PACKAGE_REDIRECTS_DIR:STATIC=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-dbg/CMakeFiles/pkgRedirects

//No help, variable specified on the command line.
CMAKE_INSTALL_BINDIR:STRING=bin

//No help, variable specified on the command line.
CMAKE_INSTALL_LIBDIR:STRING=lib

...
Skipped 245 lines
...
ZMQ_BUILD_TESTS:UNINITIALIZED=OFF

//Value Computed by CMake
ZeroMQ_BINARY_DIR:STATIC=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-dbg

//Value Computed by CMake
ZeroMQ_IS_TOP_LEVEL:STATIC=ON

//Value Computed by CMake
ZeroMQ_SOURCE_DIR:STATIC=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/src/v4.3.5-37c87b6b8c.clean

//The directory which contains the installed libraries for each
// triplet
_VCPKG_INSTALLED_DIR:PATH=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed

//No help, variable specified on the command line.
_VCPKG_ROOT_DIR:UNINITIALIZED=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg


########################
# INTERNAL cache entries
########################

//ADVANCED property for variable: CMAKE_AR
CMAKE_AR-ADVANCED:INTERNAL=1
//This is the directory where this CMakeCache.txt was created
CMAKE_CACHEFILE_DIR:INTERNAL=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/x64-windows-dbg
//Major version of cmake used to create the current loaded cache
CMAKE_CACHE_MAJOR_VERSION:INTERNAL=3
//Minor version of cmake used to create the current loaded cache
CMAKE_CACHE_MINOR_VERSION:INTERNAL=29
//Patch version of cmake used to create the current loaded cache
CMAKE_CACHE_PATCH_VERSION:INTERNAL=5
//Path to CMake executable.
CMAKE_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
//Path to cpack program executable.
CMAKE_CPACK_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cpack.exe
//Path to ctest program executable.
CMAKE_CTEST_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe
//ADVANCED property for variable: CMAKE_CXX_COMPILER
CMAKE_CXX_COMPILER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_COMPILER
CMAKE_C_COMPILER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS
CMAKE_C_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_DEBUG
CMAKE_C_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_MINSIZEREL
CMAKE_C_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELEASE
CMAKE_C_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELWITHDEBINFO
CMAKE_C_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_STANDARD_LIBRARIES
CMAKE_C_STANDARD_LIBRARIES-ADVANCED:INTERNAL=1
//Executable file format
CMAKE_EXECUTABLE_FORMAT:INTERNAL=Unknown
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS
CMAKE_EXE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_DEBUG
CMAKE_EXE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELEASE
CMAKE_EXE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXPORT_COMPILE_COMMANDS
CMAKE_EXPORT_COMPILE_COMMANDS-ADVANCED:INTERNAL=1
//Name of external makefile project generator.
CMAKE_EXTRA_GENERATOR:INTERNAL=
//Name of generator.
CMAKE_GENERATOR:INTERNAL=Ninja
//Generator instance identifier.
CMAKE_GENERATOR_INSTANCE:INTERNAL=
//Name of generator platform.
CMAKE_GENERATOR_PLATFORM:INTERNAL=
//Name of generator toolset.
CMAKE_GENERATOR_TOOLSET:INTERNAL=
//Source directory with the top level CMakeLists.txt file for this
// project
CMAKE_HOME_DIRECTORY:INTERNAL=//Desktop-8n7nb0d/d/UnrealProjects/C7_building_UE4/vcpkg_installed/vcpkg/blds/zeromq/src/v4.3.5-37c87b6b8c.clean
//ADVANCED property for variable: CMAKE_LINKER
CMAKE_LINKER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS
CMAKE_MODULE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_DEBUG
CMAKE_MODULE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL
CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELEASE
CMAKE_MODULE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MT
CMAKE_MT-ADVANCED:INTERNAL=1
//number of local generators
CMAKE_NUMBER_OF_MAKEFILES:INTERNAL=1
//Platform information initialized
CMAKE_PLATFORM_INFO_INITIALIZED:INTERNAL=1
//noop for ranlib
CMAKE_RANLIB:INTERNAL=:
//ADVANCED property for variable: CMAKE_RC_COMPILER
CMAKE_RC_COMPILER-ADVANCED:INTERNAL=1
CMAKE_RC_COMPILER_WORKS:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS
CMAKE_RC_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_DEBUG
CMAKE_RC_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_MINSIZEREL
CMAKE_RC_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELEASE
CMAKE_RC_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELWITHDEBINFO
CMAKE_RC_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//Path to CMake installation.
CMAKE_ROOT:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS
CMAKE_SHARED_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_DEBUG
CMAKE_SHARED_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELEASE
CMAKE_SHARED_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_INSTALL_RPATH
CMAKE_SKIP_INSTALL_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_RPATH
CMAKE_SKIP_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS
CMAKE_STATIC_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_DEBUG
CMAKE_STATIC_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL
CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELEASE
CMAKE_STATIC_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_TOOLCHAIN_FILE
CMAKE_TOOLCHAIN_FILE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_VERBOSE_MAKEFILE
CMAKE_VERBOSE_MAKEFILE-ADVANCED:INTERNAL=1
//Install the dependencies listed in your manifest:
//\n    If this is off, you will have to manually install your dependencies.
//\n    See https://github.com/microsoft/vcpkg/tree/master/docs/specifications/manifests.md
// for more info.
//\n
VCPKG_MANIFEST_INSTALL:INTERNAL=OFF
//ADVANCED property for variable: VCPKG_VERBOSE
VCPKG_VERBOSE-ADVANCED:INTERNAL=1
//Making sure VCPKG_MANIFEST_MODE doesn't change
Z_VCPKG_CHECK_MANIFEST_MODE:INTERNAL=OFF
//Vcpkg root directory
Z_VCPKG_ROOT_DIR:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg

```
</details>

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "dependencies": [
    "zeromq",
    "cppzmq"
  ],
  "builtin-baseline": "5e5d0e1cd7785623065e77eff011afdeec1a3574"
}

```
</details>
