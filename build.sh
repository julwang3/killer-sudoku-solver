#!/bin/bash

set -e
mkdir -p web/build

em++ \
  src/bindings.cpp \
  src/core/solver_json.cpp \
  src/solver/sudoku_solver.cpp \
  src/solver/killer_sudoku_solver.cpp \
  -Iincludes \
  -Isrc \
  -o web/build/logic.js \
  -lembind \
  -s WASM=1 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME=createModule \
  -O3

echo "Built build/logic.js + build/logic.wasm"