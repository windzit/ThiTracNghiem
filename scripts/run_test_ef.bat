@echo off
cd /d "%~dp0\.."
echo ============================================================
echo   CHAY BO TEST FULL CHO PHAN E (MON HOC) VA F (CAU HOI)
echo ============================================================
.\build\Debug\server.exe --test-ef
echo.
pause
