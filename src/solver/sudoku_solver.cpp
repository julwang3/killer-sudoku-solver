#include "sudoku_solver.h"

#include "../core/sudoku.h"

bool SudokuSolver::couldPlace(int num, int row, int col) const
{
    int subboxIndex = (row / n) * n + (col / n);
    return !m_rows[row][num - 1] && !m_cols[col][num - 1] && !m_subboxes[subboxIndex][num - 1];
}

void SudokuSolver::placeNumber(int num, int row, int col)
{
    int subboxIndex = (row / n) * n + (col / n);

    m_board[row][col] = static_cast<char>(num + '0');
    m_rows[row][num - 1] = true;
    m_cols[col][num - 1] = true;
    m_subboxes[subboxIndex][num - 1] = true;
}

void SudokuSolver::removeNumber(int num, int row, int col)
{
    int subboxIndex = (row / n) * n + (col / n);

    m_board[row][col] = '.';
    m_rows[row][num - 1] = false;
    m_cols[col][num - 1] = false;
    m_subboxes[subboxIndex][num - 1] = false;
}

bool SudokuSolver::backtrack(int row, int col)
{
    // Solved sudoku
    if (row == N)
    {
        return true;
    }

    // Iterate on next cell
    int nextRow = (col == 8) ? row + 1 : row;
    int nextCol = (col == 8) ? 0 : col + 1;
    if (m_board[row][col] != '.')
    {
        return backtrack(nextRow, nextCol);
    }

    // Try placing all available numbers
    for (int num = 1; num <= N; num++)
    {
        if (couldPlace(num, row, col))
        {
            placeNumber(num, row, col);
            if (backtrack(nextRow, nextCol))
            {
                // Stop if a solution is found
                return true;
            }
            removeNumber(num, row, col); // Invalid solution
        }
    }

    // No solution found -> Backtrack further
    return false;
}

bool SudokuSolver::solve(std::vector<std::vector<char>>& board)
{
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