#pragma once

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

/**
 * @brief Parse "board" input in JSON, returns true if input is valid
 * 
 * @param board     9x9 grid, containing the board array
 * @param input     JSON input
 * @param output    JSON output
 */
bool parseBoard(std::vector<std::vector<char>> &board, const json &input, json &output);

/**
 * @brief Parse "cages" input in JSON, returns true if input is valid
 * 
 * @param cageIds   9x9 grid, containing the correspoinding cage ID
 * @param cageSum   List containing the cage ID sums
 * @param input     JSON input
 * @param output    JSON output
 */
bool parseCages(std::vector<std::vector<int>> &cageIds, std::vector<int> &cageSum, const json &input, json &output);

/**
 * @brief Checks that every row, column, and 3x3 box contains each of 1-9 exactly once (standard Sudoku)
 */
bool isValidSudokuBoard(const std::vector<std::vector<char>> &board);

/**
 * @brief Checks for valid sudoku board AND values in cages sum to correct cage sum
 */
bool isValidKillerSudokuBoard(
    const std::vector<std::vector<char>> &board,
    const std::vector<std::vector<int>> &cageIds,
    const std::vector<int> &cageSum);

/**
 * @brief Solve standard sudoku given input (input and output both in JSON)
 *
 * @param inputJson     Input:          {"board": [["5","3", ...], ["6",".", ...], ... 9 rows total]}
 * @return std::string  Output Success: {"solved": true, "board": [...9 solved rows...]}
 *                      Output Failure: {"solved": false, "error": "<message>"}
 */
std::string solveSudokuJson(const std::string &inputJson);

/**
 * @brief Solve killer sudoku given input (input and output both in JSON)
 *
 * @param inputJson     Input:
 *                      {
 *                          "board": [9 rows, each an array of 9 single-character cells],
 *                          "cages": [ { "sum": <int>, "cells": [[row, col], ...] }, ... ]
 *                      }
 * @return std::string  Output Success: {"solved": true, "board": [...9 solved rows...]}
 *                      Output Failure: {"solved": false, "error": "<message>"}
 */
std::string solveKillerSudokuJson(const std::string &inputJson);