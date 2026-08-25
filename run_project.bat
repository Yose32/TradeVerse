@echo off
echo ========================================================
echo   TradeVerse - Full-Stack C++ DSA Trading Simulator
echo ========================================================
echo.
echo Starting C++ Backend Server (Port 8080)...
start "TradeVerse C++ Backend" cmd /k "cd backend && tradeverse_server.exe"

timeout /t 2 /nobreak >nul

echo Starting Frontend Web Dashboard (Port 3000)...
start "TradeVerse Frontend" cmd /k "cd frontend && npm run dev"

echo.
echo TradeVerse is launching!
echo Access the web UI at: http://localhost:3000
echo Backend API at: http://localhost:8080
echo ========================================================
pause