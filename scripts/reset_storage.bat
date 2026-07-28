@echo off
echo === Reset Storage to Default Empty State ===
echo.
echo This will:
echo   1. Auto-backup current storage/ to storage_backup_reset_<timestamp>/
echo   2. Reset 9 flat files to header-only (empty) state
echo.
echo Press any key to continue, or Ctrl+C to cancel...
pause > nul
echo.
build\Debug\server.exe --reset-storage
echo.
pause