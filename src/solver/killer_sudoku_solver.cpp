#include "killer_sudoku_solver.h"

#include "../core/sudoku.h"

bool KillerSudokuSolver::solve(
    std::vector<std::vector<char>> &board,
    std::vector<std::vector<int>> &cageIds,
    std::vector<int> &cageSum
)
{
    // TODO: Update for Killer Sudoku
    
    // Initialize variables
    m_board = board;
    m_rows.assign(N, std::vector<int>(N, 0));
    m_cols.assign(N, std::vector<int>(N, 0));
    m_subboxes.assign(N, std::vector<int>(N, 0));

    // Initialize board
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
        {
            if (m_board[r][c] != '.')
            {
                placeNumber(m_board[r][c] - '0', r, c);
            }
        }
    }

    // Perform backtracking
    bool solved = backtrack(0, 0);
    if (solved)
    {
        board = m_board;
    }
    return solved;
}