#include "automata2d.hpp"
#include <functional>

Automata2D::Automata2D(size_t rows, size_t cols) : grid(rows, std::vector<int>(cols, 0)) {}

Automata2D::Automata2D(const std::vector<std::vector<int>>& initialState) : grid(initialState) {}

void Automata2D::setState(size_t row, size_t col, int value) {
    grid[row][col] = value;
}

int Automata2D::getState(size_t row, size_t col) const {
    return grid[row][col];
}

size_t Automata2D::getRows() const {
    return grid.size();
}

size_t Automata2D::getCols() const {
    return grid.empty() ? 0 : grid[0].size();
}

int Automata2D::countMooreNeighbors(size_t row, size_t col) const {
    int count = 0;
    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());
    
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = static_cast<int>(row) + dr;
            int nc = static_cast<int>(col) + dc;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                count += grid[nr][nc];
            }
        }
    }
    return count;
}

int Automata2D::countVonNeumannNeighbors(size_t row, size_t col) const {
    int count = 0;
    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());
    
    int dr[] = {-1, 0, 1, 0};
    int dc[] = {0, 1, 0, -1};
    
    for (int i = 0; i < 4; i++) {
        int nr = static_cast<int>(row) + dr[i];
        int nc = static_cast<int>(col) + dc[i];
        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
            count += grid[nr][nc];
        }
    }
    return count;
}

void Automata2D::applyRule(std::function<int(const Automata2D&, size_t, size_t)> rule) {
    auto newGrid = grid;
    for (size_t r = 0; r < grid.size(); r++) {
        for (size_t c = 0; c < grid[r].size(); c++) {
            newGrid[r][c] = rule(*this, r, c);
        }
    }
    grid = newGrid;
}

std::vector<std::vector<int>> Automata2D::getMatrix() const {
    return grid;
}