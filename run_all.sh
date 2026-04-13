#!/usr/bin/env bash
# Smart Farming runner: starts frontend server and backend app

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_ROOT"

# ── Frontend ──────────────────────────────────────────────────────────────────
SERVER_PID=""

echo "Starting frontend on http://localhost:5500/index.html ..."

if command -v python3 &>/dev/null; then
    python3 -m http.server 5500 &>/dev/null &
    SERVER_PID=$!
    echo "Frontend server PID: $SERVER_PID"
    echo "Open http://localhost:5500/index.html in your browser."
elif command -v python &>/dev/null; then
    python -m SimpleHTTPServer 5500 &>/dev/null &
    SERVER_PID=$!
    echo "Frontend server PID: $SERVER_PID"
    echo "Open http://localhost:5500/index.html in your browser."
else
    echo "Warning: python not found. Frontend server not started."
fi

# ── Backend (C++) ─────────────────────────────────────────────────────────────
COMPILER=""
if command -v g++ &>/dev/null; then
    COMPILER="g++"
elif command -v clang++ &>/dev/null; then
    COMPILER="clang++"
fi

if [ -n "$COMPILER" ]; then
    echo "Compiling backend with $COMPILER ..."
    $COMPILER -std=c++17 -o smart_farming main.cpp
    echo "Backend compiled. Starting interactive session ..."
    ./smart_farming
else
    echo "No C++ compiler found. Frontend is running, but backend cannot be started."
    echo "Install g++ or clang++ and re-run this script."
    if [ -n "$SERVER_PID" ]; then
        echo ""
        echo "Press Ctrl+C to stop the frontend server."
        wait "$SERVER_PID"
    fi
fi

# ── Cleanup ───────────────────────────────────────────────────────────────────
if [ -n "$SERVER_PID" ]; then
    echo "Stopping frontend server (PID $SERVER_PID) ..."
    kill "$SERVER_PID" 2>/dev/null || true
fi
