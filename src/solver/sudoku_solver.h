#pragma once

#include <array>
#include <vector>

/** 
 * Standard Sudoku Solver based on LC#37
 */
class SudokuSolver
{
public:
    /**
     * @brief Solve the given board
     * 
     * @param board     Represents the board as a 9x9 grid, using '1'-'9' for filled or '.' for empty cells
     * @return true     Solution is found
     * @return false    Board is unsolvable
     */
    bool solve(std::vector<std::vector<char>> &board);

protected:
    bool virtual couldPlace(int num, int row, int col) const;
    void virtual placeNumber(int num, int row, int col);
    void virtual removeNumber(int num, int row, int col);
    
    bool backtrack(int row, int col);

    std::vector<std::vector<char>> m_board;
    std::vector<std::vector<int>> m_rows;
    std::vector<std::vector<int>> m_cols;
    std::vector<std::vector<int>> m_subboxes;
};