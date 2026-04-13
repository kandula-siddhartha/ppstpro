#!/usr/bin/env bash
# Smart Farming Monitoring System — Linux/macOS runner
# Starts the frontend HTTP server and compiles + runs the C++ backend.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

FRONTEND_PORT=5500
BACKEND_BIN="./smart_farming_app"
FRONTEND_PID=""

# ── Frontend ────────────────────────────────────────────────────────────────
echo "Starting frontend on http://localhost:${FRONTEND_PORT}/index.html ..."
if command -v python3 &>/dev/null; then
    python3 -m http.server "$FRONTEND_PORT" &>/dev/null &
    FRONTEND_PID=$!
elif command -v python &>/dev/null; then
    python -m http.server "$FRONTEND_PORT" &>/dev/null &
    FRONTEND_PID=$!
else
    echo "WARNING: Python not found. Cannot start frontend HTTP server."
fi
if [ -n "$FRONTEND_PID" ]; then
    echo "  Frontend PID: ${FRONTEND_PID}"
    echo "  Open http://localhost:${FRONTEND_PORT}/index.html in your browser."
fi

# ── Backend ─────────────────────────────────────────────────────────────────
if command -v g++ &>/dev/null; then
    COMPILER="g++"
elif command -v clang++ &>/dev/null; then
    COMPILER="clang++"
else
    echo "WARNING: No C++ compiler (g++ / clang++) found. Backend will not be started."
    echo "  Install g++: sudo apt install g++ (Debian/Ubuntu) or brew install gcc (macOS)"
    if [ -n "$FRONTEND_PID" ]; then
        echo "  Frontend server (PID ${FRONTEND_PID}) is still running. Press Ctrl+C to stop it."
        wait "$FRONTEND_PID"
    fi
    exit 0
fi

echo "Compiling backend with ${COMPILER} ..."
"$COMPILER" -std=c++17 -Wall main.cpp -o smart_farming_app
echo "  Compilation successful."

echo "Starting backend CLI ..."
echo "  (The backend is a terminal application. Use the menu to manage farms, crops, and sensors.)"
echo ""
"$BACKEND_BIN"

# When backend exits, stop the frontend server
echo ""
if [ -n "$FRONTEND_PID" ]; then
    echo "Backend exited. Stopping frontend server (PID ${FRONTEND_PID}) ..."
    kill "$FRONTEND_PID" 2>/dev/null || true
fi
