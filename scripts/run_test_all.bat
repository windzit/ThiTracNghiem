@echo off
cd /d "%~dp0\.."
echo ============================================================
echo   CHAY TOAN BO TEST SUITE (VALIDATION + E & F INTEGRATION)
echo ============================================================
.\build\Debug\server.exe --test-all
echo.
pause
