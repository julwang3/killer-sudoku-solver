#include "solver_json.h"

#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#include "sudoku.h"
#include "../solver/sudoku_solver.h"
#include "../solver/killer_sudoku_solver.h"

using json = nlohmann::json;

bool parseBoard(std::vector<std::vector<char>> &board, const json &input, json &output)
{
    board.reserve(N);
    for (const auto &row : input["board"])
    {
        if (!row.is_array() || row.size() != 9)
        {
            output["solved"] = false;
            output["error"] = "Each row must be an array of 9 single-character cells";
            return false;
        }

        std::vector<char> parsedRow;
        parsedRow.reserve(N);
        for (const auto &cell : row)
        {
            if (!cell.is_string() || cell.get<std::string>().size() != 1)
            {
                output["solved"] = false;
                output["error"] = "Each cell must be a single-character string ('.' or '1'-'9')";
                return false;
            }
            parsedRow.push_back(cell.get<std::string>()[0]);
        }
        board.push_back(std::move(parsedRow));
    }
    return true;
}

bool parseCages(std::vector<std::vector<int>> &cageIds, std::vector<int> &cageSum, const json &input, json &output)
{
    cageSum.reserve(input["cages"].size());

    int cellsCovered = 0;
    int cageIndex = 0;
    for (const auto &cage : input["cages"])
    {
        if (!cage.is_object() || !cage.contains("sum") || !cage["sum"].is_number_integer() ||
            !cage.contains("cells") || !cage["cells"].is_array() || cage["cells"].empty())
        {
            output["solved"] = false;
            output["error"] = "Each cage must be {\"sum\": <int>, \"cells\": [[row, col], ...]}";
            return false;
        }

        for (const auto &cellCoord : cage["cells"])
        {
            if (!cellCoord.is_array() || cellCoord.size() != 2 ||
                !cellCoord[0].is_number_integer() || !cellCoord[1].is_number_integer())
            {
                output["solved"] = false;
                output["error"] = "Each cage cell must be [row, col]";
                return false;
            }

            int r = cellCoord[0].get<int>();
            int c = cellCoord[1].get<int>();
            if (r < 0 || r >= N || c < 0 || c >= N)
            {
                output["solved"] = false;
                output["error"] = "Cage cell coordinates out of range (must be 0-8)";
                return false;
            }
            if (cageIds[r][c] != -1)
            {
                output["solved"] = false;
                output["error"] = "Cell (" + std::to_string(r) + "," + std::to_string(c) +
                                  ") is assigned to more than one cage";
                return false;
            }

            cageIds[r][c] = cageIndex;
            cellsCovered++;
        }

        cageSum.push_back(cage["sum"].get<int>());
        cageIndex++;
    }

    if (cellsCovered != N * N)
    {
        output["solved"] = false;
        output["error"] = "cages must fully partition the board (all 81 cells covered exactly once)";
        return false;
    }

    return true;
}

bool isValidSudokuBoard(const std::vector<std::vector<char>> &board)
{
    if (board.size() != N)
        return false;

    std::vector<std::vector<int>> rows(N, std::vector<int>(N));
    std::vector<std::vector<int>> cols(N, std::vector<int>(N));
    std::vector<std::vector<int>> subboxes(N, std::vector<int>(N));

    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
        {
            if (board[r][c] < '1' || board[r][c] > '9')
            {
                return false;
            }

            int num = board[r][c] - '1';
            int subboxIndex = (r / n) * n + (c / n);
            if (rows[r][num] || cols[c][num] ||
                subboxes[subboxIndex][num])
            {
                return false;
            }
            rows[r][num] = true;
            cols[c][num] = true;
            subboxes[subboxIndex][num] = true;
        }
    }

    return true;
}

bool isValidKillerSudokuBoard(
    const std::vector<std::vector<char>> &board,
    const std::vector<std::vector<int>> &cageIds,
    const std::vector<int> &cageSum)
{
    if (!isValidSudokuBoard(board))
        return false;

    // Parse board
    int sums = cageSum.size();
    std::vector<int> boardCageSum(sums, 0);
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
        {
            int cageId = cageIds[r][c];
            int num = board[r][c] - '0';

            if (cageId >= sums || (boardCageSum[cageId] + num) > cageSum[cageId])
                return false;

            boardCageSum[cageId] += num;
        }
    }

    // Check cages
    for (int i = 0; i < sums; i++)
    {
        if (boardCageSum[i] != cageSum[i])
            return false;
    }
    return true;
}

std::string solveSudokuJson(const std::string &inputJson)
{
    json input;
    json output;

    // Parse input JSON
    try
    {
        input = json::parse(inputJson);
    }
    catch (const json::parse_error &e)
    {
        output["solved"] = false;
        output["error"] = std::string("Invalid JSON: ") + e.what();
        return output.dump();
    }

    // Verify input
    if (!input.contains("board") || !input["board"].is_array() || input["board"].size() != 9)
    {
        output["solved"] = false;
        output["error"] = "Expected {\"board\": [9 strings of length 9]}";
        return output.dump();
    }

    // Define input board
    std::vector<std::vector<char>> board(9);
    if (!parseBoard(board, input, output))
        return output.dump();

    // Solve sudoku
    SudokuSolver solver;
    bool solved = solver.solve(board);

    // Create output JSON
    output["solved"] = solved;
    if (solved)
    {
        json boardJson = json::array();
        for (const auto &row : board)
        {
            json rowJson = json::array();
            for (char c : row)
            {
                rowJson.push_back(std::string(1, c));
            }
            boardJson.push_back(rowJson);
        }
        output["board"] = boardJson;
    }
    else
    {
        output["error"] = "no solution exists for this board";
    }
    return output.dump();
}

std::string solveKillerSudokuJson(const std::string &inputJson)
{
    json input;
    json output;

    // Parse input JSON
    try
    {
        input = json::parse(inputJson);
    }
    catch (const json::parse_error &e)
    {
        output["solved"] = false;
        output["error"] = std::string("Invalid JSON: ") + e.what();
        return output.dump();
    }

    // Verify input
    if (!input.contains("board") || !input["board"].is_array() || input["board"].size() != N)
    {
        output["solved"] = false;
        output["error"] = "Expected {\"board\": [9 rows, each an array of 9 single-character cells], \"cages\": [...]}";
        return output.dump();
    }
    if (!input.contains("cages") || !input["cages"].is_array() || input["cages"].empty())
    {
        output["solved"] = false;
        output["error"] = "Expected \"cages\": [{\"sum\": <int>, \"cells\": [[row, col], ...]}, ...]";
        return output.dump();
    }

    // Define input board
    std::vector<std::vector<char>> board;
    if (!parseBoard(board, input, output))
        return output.dump();

    // Define cages
    std::vector<std::vector<int>> cageIds(N, std::vector<int>(N, -1));
    std::vector<int> cageSum;
    if (!parseCages(cageIds, cageSum, input, output))
        return output.dump();

    // Solve killer sudoku
    KillerSudokuSolver solver;
    bool solved = solver.solve(board, cageIds, cageSum);

    // Create output JSON
    output["solved"] = solved;
    if (solved)
    {
        json boardJson = json::array();
        for (const auto &row : board)
        {
            json rowJson = json::array();
            for (char c : row)
            {
                rowJson.push_back(std::string(1, c));
            }
            boardJson.push_back(rowJson);
        }
        output["board"] = boardJson;
    }
    else
    {
        output["error"] = "No solution exists for this board/cage configuration";
    }
    return output.dump();
}
