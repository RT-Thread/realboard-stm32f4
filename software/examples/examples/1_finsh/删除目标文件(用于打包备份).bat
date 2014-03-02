del /Q build\*.* /s /q /f
del /Q Obj\*.plg
del /Q *.bak
del /Q *.dep
del /Q *.Obj
del /Q *.List
del /Q *.Administrator
del /Q *.pyc
del /Q *.bin
del /Q *.uvopt
del /Q *.sconsign.dblite
del /Q *.gitignore
del /Q *.ini
del /Q *.map
del /Q *.axf
del /Q tmpcmd.txt
del /Q JLinkLog.txt
@echo off
for /f "delims=" %%a in ('dir . /b /ad /s ^|sort /r' ) do rd /q "%%a" 2>nul



