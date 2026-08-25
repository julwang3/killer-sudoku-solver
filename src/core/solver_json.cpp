#include "solver_json.h"

#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#include "sudoku.h"
#include "../solver/sudoku_solver.h"
#include "../solver/killer_sudoku_solver.h"

using json = nlohmann::json;

bool isValidSudokuBoard(const std::vector<std::vector<char>> &board)
{
    if (board.size() != 9)
        return false;

    bool rows[9][9];
    bool cols[9][9];
    bool subboxes[9][9];
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

    if (!input.contains("board") || !input["board"].is_array() || input["board"].size() != 9)
    {
        output["solved"] = false;
        output["error"] = "Expected {\"board\": [9 strings of length 9]}";
        return output.dump();
    }

    // Define input board
    std::vector<std::vector<char>> board;
    board.reserve(9);
    for (const auto &row : input["board"])
    {
        if (!row.is_array() || row.size() != 9)
        {
            output["solved"] = false;
            output["error"] = "each row must be an array of 9 single-character cells";
            return output.dump();
        }

        std::vector<char> parsedRow;
        parsedRow.reserve(9);
        for (const auto &cell : row)
        {
            if (!cell.is_string() || cell.get<std::string>().size() != 1)
            {
                output["solved"] = false;
                output["error"] = "each cell must be a single-character string ('.' or '1'-'9')";
                return output.dump();
            }
            parsedRow.push_back(cell.get<std::string>()[0]);
        }
        board.push_back(std::move(parsedRow));
    }

    // Solve sudoku
    SudokuSolver solver;
    bool solved = solver.solve(board);

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