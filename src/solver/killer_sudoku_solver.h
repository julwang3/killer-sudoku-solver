#pragma once

#include <array>
#include <vector>

#include "sudoku_solver.h"

/**
 * Killer Sudoku Solver
 */
class KillerSudokuSolver : SudokuSolver
{
public:
    /**
     * @brief Solve the given board
     *
     * @param board     9x9 grid, '1'-'9' for filled or '.' for empty cells
     * @param cageIds   9x9 grid, where cageIds[r][c] = Index into cageSums
     * @param cageSum   Target sum for each cage
     * @return true     Solution is found
     * @return false    Board is unsolvable
     */
    bool solve(
        std::vector<std::vector<char>> &board,
        std::vector<std::vector<int>> &cageIds,
        std::vector<int> &cageSum);

protected:
    bool couldPlace(int num, int row, int col) const override;
    void placeNumber(int num, int row, int col) override;
    void removeNumber(int num, int row, int col) override;

private:
    std::vector<std::vector<int>> m_cageIds;
    std::vector<int> m_cageSum;
};