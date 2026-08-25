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
 * @brief Builds the default output path for a given input path:
 *        out/results/out_<input filename>
 *        e.g. tests/test1.json -> out/results/out_test1.json
 *
 * Kept separate from out/ build artifacts (the compiled binary) so
 * solver results don't mix with build output.
 */
fs::path defaultOutputPath(const fs::path &inputPath)
{
    return fs::path("out") / "results" / ("out_" + inputPath.filename().string());
}

/**
 * @brief Main Usage:
 *              ./test <input.json> [output.json]
 *
 * Reads a puzzle from the input file, shaped like:
 *      {"board": [["5","3",".",...], ... 9 rows, each 9 single-char cells]}
 *
 * Solves it and writes the result to the output file (or, if omitted,
 * "out/results/out_<input filename>").
 */
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input.json> [output.json]\n";
        return 1;
    }

    fs::path inputPath = argv[1];
    fs::path outputPath = (argc > 2) ? fs::path(argv[2]) : defaultOutputPath(inputPath);

    if (!fs::exists(inputPath))
    {
        std::cerr << "Input file not found: " << inputPath << "\n";
        return 1;
    }

    std::string inputJson = readFile(inputPath);
    std::string outputJson = solveSudokuJson(inputJson);

    json parsed = json::parse(outputJson); // solveSudokuJson always returns valid JSON
    writeFile(outputPath, parsed.dump(4));

    bool solved = parsed.value("solved", false);
    std::cout << (solved ? "Solved" : "Not solved") << " -> " << outputPath.string() << "\n";

    return solved ? 0 : 1;
}