#include "killer_sudoku_solver.h"

#include "../core/sudoku.h"

bool KillerSudokuSolver::couldPlace(int num, int row, int col) const
{
    if (num > m_cageSum[m_cageIds[row][col]])
    {
        return false;
    }
    return SudokuSolver::couldPlace(num, row, col);
}

void KillerSudokuSolver::placeNumber(int num, int row, int col)
{
    m_cageSum[m_cageIds[row][col]] -= num;
    SudokuSolver::placeNumber(num, row, col);
}

void KillerSudokuSolver::removeNumber(int num, int row, int col)
{
    m_cageSum[m_cageIds[row][col]] += num;
    SudokuSolver::removeNumber(num, row, col);
}

bool KillerSudokuSolver::solve(
    std::vector<std::vector<char>> &board,
    std::vector<std::vector<int>> &cageIds,
    std::vector<int> &cageSum
)
{
    // Initialize variables
    m_board = board;
    m_cageIds = cageIds;
    m_cageSum = cageSum;

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