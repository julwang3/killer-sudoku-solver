#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>

#include "../src/core/solver_json.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

/**
 * @brief Read file from specified path
 */
std::string readFile(const fs::path &path)
{
    std::ifstream in(path);
    if (!in)
    {
        throw std::runtime_error("could not open file: " + path.string());
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

/**
 * @brief Write contents to specified path, creating parent directories
 *        if they don't already exist
 */
void writeFile(const fs::path &path, const std::string &contents)
{
    if (path.has_parent_path())
    {
        fs::create_directories(path.parent_path());
    }
    std::ofstream out(path);
    out << contents;
}

/**
 * @brief Builds the default output path for a given input path and mode:
 *        out/results/out_sudoku_<input filename>   (mode == "sudoku")
 *        out/results/out_ksudoku_<input filename>  (mode == "killer")
 *        e.g. tests/test1.json -> out/results/out_ksudoku_test1.json
 *
 * Kept separate from out/ build artifacts (the compiled binary) so
 * solver results don't mix with build output.
 */
fs::path defaultOutputPath(const fs::path &inputPath, const std::string &mode)
{
    std::string prefix = (mode == "killer") ? "out_ksudoku_" : "out_sudoku_";
    return fs::path("out") / "results" / (prefix + inputPath.filename().string());
}

/**
 * @brief Prints usage instructions to stderr
 */
void printUsage(const char *programName)
{
    std::cerr << "Usage: " << programName << " <sudoku|killer> <input.json> [output.json]\n";
}

/**
 * @brief Main Usage:
 *              ./test <sudoku|killer> <input.json> [output.json]
 *
 * mode "sudoku": reads a standard puzzle, shaped like:
 *      {"board": [["5","3",".",...], ... 9 rows, each 9 single-char cells]}
 *
 * mode "killer": reads a killer sudoku puzzle, shaped like:
 *      {"board": [...same as above...],
 *       "cages": [{"sum": <int>, "cells": [[row, col], ...]}, ...]}
 *
 * Solves it and writes the result to the output file (or, if omitted,
 * "out/results/out_<input filename>").
 */
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printUsage(argv[0]);
        return 1;
    }

    std::string mode = argv[1];
    fs::path inputPath = argv[2];

    if (mode != "sudoku" && mode != "killer")
    {
        std::cerr << "Unknown mode: \"" << mode << "\" (expected \"sudoku\" or \"killer\")\n";
        printUsage(argv[0]);
        return 1;
    }

    fs::path outputPath = (argc > 3) ? fs::path(argv[3]) : defaultOutputPath(inputPath, mode);

    if (!fs::exists(inputPath))
    {
        std::cerr << "Input file not found: " << inputPath << "\n";
        return 1;
    }

    std::string inputJson = readFile(inputPath);

    std::string outputJson = (mode == "sudoku")
        ? solveSudokuJson(inputJson)
        : solveKillerSudokuJson(inputJson);

    json parsed = json::parse(outputJson); // solve*Json always returns valid JSON
    writeFile(outputPath, parsed.dump(4));

    bool solved = parsed.value("solved", false);
    std::cout << "[" << mode << "] " << (solved ? "Solved" : "Not solved")
              << " -> " << outputPath.string() << "\n";

    return solved ? 0 : 1;
}