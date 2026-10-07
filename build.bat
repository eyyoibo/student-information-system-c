@echo off
REM =============================================================================
REM Student Information System - Build Script (Windows)
REM =============================================================================
REM Usage:
REM   build.bat         - Build main program
REM   build.bat test    - Build and run tests
REM   build.bat clean   - Remove compiled files
REM   build.bat run     - Build and run main program
REM =============================================================================

if "%1"=="" goto main
if "%1"=="test" goto test
if "%1"=="clean" goto clean
if "%1"=="run" goto run
goto help

:main
echo [BUILD] Compiling main program...
gcc -Wall -Wextra -std=c99 -o sis.exe src/main.c src/utils.c src/student.c src/professor.c src/course.c src/enrollment.c src/grade.c src/menu.c src/curriculum.c
if %ERRORLEVEL%==0 (
    echo [SUCCESS] Main program built: sis.exe
) else (
    echo [ERROR] Compilation failed!
)
goto end

:test
echo [BUILD] Compiling test suite...
gcc -Wall -Wextra -std=c99 -I src -o tests.exe tests/test_main.c src/utils.c src/student.c src/professor.c src/course.c src/enrollment.c src/grade.c src/menu.c src/curriculum.c tests/test_utils.c tests/test_student.c tests/test_professor.c tests/test_course.c tests/test_enrollment.c tests/test_grade.c tests/test_menu.c
if %ERRORLEVEL%==0 (
    echo [SUCCESS] Test executable built: tests.exe
    echo.
    echo [RUN] Running tests...
    echo ========================================
    tests.exe
) else (
    echo [ERROR] Compilation failed!
)
goto end

:clean
echo [CLEAN] Removing compiled files...
del /Q sis.exe tests.exe *.o 2>nul
echo [SUCCESS] Cleaned.
goto end

:run
call :main
if %ERRORLEVEL%==0 (
    echo.
    echo [RUN] Starting program...
    echo ========================================
    sis.exe
)
goto end

:help
echo.
echo Usage: build.bat [command]
echo.
echo Commands:
echo   (none)   Build main program (sis.exe)
echo   test     Build and run all tests
echo   clean    Remove compiled files
echo   run      Build and run main program
echo.
goto end

:end
