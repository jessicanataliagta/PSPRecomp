@echo off
call "%~dp0scripts\build_release_ninja_clangcl.bat"
exit /b %errorlevel%
