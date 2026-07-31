@echo off
echo === Seeding PTIT Sample Data (20 Classes, 200 Students, 20 Subjects, 200 Questions) ===
echo.
echo [0] Stopping any running server instance...
taskkill /f /im server.exe > nul 2>&1
ping 127.0.0.1 -n 2 > nul

set SERVER_EXE=build\Debug\server.exe
if not exist %SERVER_EXE% set SERVER_EXE=build\Release\server.exe
if not exist %SERVER_EXE% set SERVER_EXE=server.exe

echo [1] Resetting storage directory to clean default state...
%SERVER_EXE% --reset-storage

echo [2] Starting fresh server for seeding...
start "ThiTracNghiem Server" /min %SERVER_EXE%

echo [2.1] Waiting for API server to become ready...
:wait_server
ping 127.0.0.1 -n 2 > nul
curl -s http://localhost:8080/ > nul 2>&1
if errorlevel 1 goto wait_server

echo [3] Seeding dataset via HTTP REST API calls...
powershell -ExecutionPolicy Bypass -File scripts\seed_ptit_data.ps1

echo.
echo [4] Stopping server...
taskkill /f /im server.exe > nul 2>&1

echo.
echo === Seed Data Complete ===
echo   - 20 classes
echo   - 200 students (10 per class)
echo   - 20 subjects
echo   - 200 questions (10 per subject)
echo   - Teacher account: GV / GV
echo   - Student password: 123
echo.
pause