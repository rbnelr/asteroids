@echo off & setlocal enabledelayedexpansion
	
	cls
	color 07
	
	rem MSVC should be always in path
	set LLVM=D:/Cproj/_llvm4.0/bin/
	if [!GCC!] == [] set GCC=D:/pt_proj/tdm_gcc/bin/
	
	set ROOT=%~dp0
	set SRC=!ROOT!src/
	set DEPS=!ROOT!deps/
	set GLFW=!DEPS!glfw-3.2.1.bin.WIN64/
	set GLAD=!DEPS!glad/
	set STB=!DEPS!stb/
	
	set func=vs
	if not [%1] == []		set func=%1
	
	set release=0
	if [%2] == [dbg]		set release=0
	if [%2] == [release]	set release=1
	
	set proj=asteroids
	if not [%3] == []		set proj=%3
	
	set succ=0
	call :%func%
	
	if [succ] == [0] (
		echo fail.
	) else (
		echo success.
	)
	exit /b
rem /main

:vs
	if [!release!] == [0] (
		set dbg=/Od /Ob1 /MDd /Zi /DRZ_DBG=1
	) else  (
		set dbg=/O2 /Ob2 /MD /Zi /Zo /Oi /DRZ_DBG=0
	)
	
	set opt=!dbg! /fp:fast /GS-
	
	set warn=/wd4577
	rem /wd4577 noexcept used with no exceptions enabled
	
	rem /showIncludes
	cl.exe -nologo /source-charset:utf-8 /DRZ_PLATF=1 /DRZ_ARCH=1 !opt! !warn! /I!SRC!include /I!GLFW!include /I!GLAD! /I!STB! !SRC!!proj!.cpp /Fe!ROOT!!proj!.exe /link KERNEL32.lib OPENGL32.lib !GLFW!lib-vc2015/glfw3dll.lib /INCREMENTAL:NO /SUBSYSTEM:CONSOLE /OPT:REF
	
	del *.obj
	
	exit /b
rem /vs

:gcc
	if [!release!] == [0] (
		set dbg=-O0 -DRZ_DBG=1
	) else  (
		set dbg=-O3 -DRZ_DBG=0
	)
	
	set opt=!dbg! -mmmx -msse -msse2
	rem -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt
	
	set warn=-Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-tautological-compare
	rem -fmax-errors=5
	rem -Wno-unused-variable
	rem -Wno-unused-function
	rem -Wtautological-compare		constant if statements
	
	!GCC!g++ -std=c++11 -m64 -DRZ_PLATF=1 -DRZ_ARCH=1 !opt! !warn! -I!SRC!include -I!GLFW!include -I!GLAD! -I!STB! -o !ROOT!!proj!.exe !SRC!!proj!.cpp -L!GLFW!lib-mingw-w64 -lglfw3dll
	
	exit /b
rem /llvm

:llvm
	rem -g0 this still generates a .pdb! and is disasm'able with dumpbin but does source code steppable in visual studio
	if [!release!] == [0] (
		set dbg=-O0 -g0 -DRZ_DBG=1
	) else  (
		set dbg=-O3 -g0 -DRZ_DBG=0
	)
	
	set opt=!dbg! -mmmx -msse -msse2
	rem -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt
	
	set warn=-Wall -Wno-unused-variable -Wno-unused-function -Wno-tautological-compare
	rem -fmax-errors=5
	rem -Wno-unused-variable
	rem -Wno-unused-function
	rem -Wtautological-compare		constant if statements
	
	!LLVM!clang++ -std=c++11 -m64 -DRZ_PLATF=1 -DRZ_ARCH=1 !opt! !warn! -I!SRC!include -I!GLFW!include -I!GLAD! -I!STB! -o !ROOT!!proj!.exe !SRC!!proj!.cpp -lKERNEL32 -lUSER32 -lGDI32 -lSHELL32 -l!GLFW!lib-vc2015/glfw3dll
	
	rem !LLVM!clang++ -std=c++11 -m64 -DRZ_PLATF=1 -DRZ_ARCH=1 !opt! !warn! -I!SRC!include -I!GLFW!include !SRC!!proj!.cpp -E > out.cpp
	
	exit /b
rem /llvm

:disasm
	dumpbin /ALL /DISASM /SYMBOLS /out:!ROOT!!proj!.exe.asm.tmp !ROOT!!proj!.exe
	undname !ROOT!!proj!.exe.asm.tmp > !ROOT!!proj!.exe.asm
	del !ROOT!!proj!.exe.asm.tmp
	
	rem dumpbin /ALL /DISASM /SYMBOLS /out:!ROOT!!proj!.exe.asm !ROOT!!proj!.exe
	
	exit /b
rem /a

rem :copy_built
rem 	copy D:\Cproj\lib_engine\src\glfw-3.2.1.src\src\Release\glfw3.lib				D:\Cproj\lib_engine\src\glfw-3.2.1.built\glfw3_release.lib
rem 	copy D:\Cproj\lib_engine\src\glfw-3.2.1.src\src\glfw.dir\Release\glfw.pdb		D:\Cproj\lib_engine\src\glfw-3.2.1.built\glfw3_release.pdb
rem 	
rem 	copy D:\Cproj\lib_engine\src\glfw-3.2.1.src\src\Debug\glfw3.lib					D:\Cproj\lib_engine\src\glfw-3.2.1.built\glfw3_dbg.lib
rem 	copy D:\Cproj\lib_engine\src\glfw-3.2.1.src\src\glfw.dir\Debug\glfw.pdb			D:\Cproj\lib_engine\src\glfw-3.2.1.built\glfw3_dbg.pdb
rem 	
rem 	exit /b
rem rem /a
