@echo off
call ..\Debug\RNA-2022.exe -in:..\RNA-2022\in.txt -out:program.asm
call C:\PROGRA~2\MICROS~2\2019\Community\VC\Auxiliary\Build\vcvarsall.bat x86
ml /c program.asm
link /OPT:NOREF /OUT:program.exe /SUBSYSTEM:CONSOLE libucrt.lib program.obj
call program.exe
pause