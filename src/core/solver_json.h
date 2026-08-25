#pragma once

#include <string>

/**
 * @brief Checks that every row, column, and 3x3 box contains each of 1-9 exactly once (standard Sudoku)
 */
bool isValidSudokuBoard(const std::vector<std::vector<char>> &board);

/**
 * @brief Solve standard sudoku given input (input and output both in JSON)
 * 
 * @param inputJson     Input:          {"board": ["53..7....", "6..195...", ... 9 rows total]}
 * @return std::string  Output Success: {"solved": true, "board": [...9 solved rows...]}
 *                      Output Failure: {"solved": false, "error": "<message>"}
 */
std::string solveSudokuJson(const std::string& inputJson);