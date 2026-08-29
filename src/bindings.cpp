#include <emscripten/bind.h>
#include "core/solver_json.h"

EMSCRIPTEN_BINDINGS(sudoku_module)
{
    emscripten::function("solveSudoku", &solveSudokuJson);
    emscripten::function("solveKillerSudoku", &solveKillerSudokuJson);
}