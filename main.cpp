
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::cout;
using std::ifstream;
using std::istringstream;
using std::string;
using std::vector;
using std::sort;

enum class State { kEmpty, kObstacle, kClosed, kPath, kStart, kFinish };

/**
 * Check if a given cell is an allowed move.
 */
bool CheckValidCell(int x, int y, vector<vector<State>> grid) {
  if (x < 0 or y < 0) return false;

  if (x >= grid.size() || y >= grid[0].size()) return false;

  if (grid[x][y] == State::kEmpty) return true; else return false;
}

/**
 * Add a node to the open list and mark it as open.
 */
void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &open,
               vector<vector<State>> &grid) {
  vector<int> node = {x, y, g, h};

  // Add node to open vector, and mark grid cell as closed.
  open.push_back(node);
  grid[x][y] = State::kClosed;
}

/**
 * Compare the F values of two cells.
 */
bool Compare(vector<int> a, vector<int> b) {
  return ((a[2] + a[3]) > (b[2] + b[3]));
}

/**
 * Sort the two-dimensional vector of ints in descending order.
 */
void CellSort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), Compare);
}

/**
 * Parse a line of the board.
 */
vector<State> ParseLine(string line) {
  istringstream sline(line);
  int n;
  char c;
  vector<State> row;
  while (sline >> n >> c && c == ',') {
    if (n == 0) {
      row.push_back(State::kEmpty);
    } else {
      row.push_back(State::kObstacle);
    }
  }
  return row;
}

/*
 * Read a 2-dimensional State board from a file path.
 */
vector<vector<State>> ReadBoardFile(string path) {
  ifstream myfile(path);
  vector<vector<State>> board{};
  if (myfile) {
    string line;
    while (getline(myfile, line)) {
      vector<State> row = ParseLine(line);
      board.push_back(row);
    }
  }
  return board;
}

/**
 * Heuristic function.
 * Calculate the manhattan distance.
 */
int Heuristic(int x1, int y1, int x2, int y2) {
  return std::abs(x2 - x1) + std::abs(y2 - y1);
}

// directional deltas
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

/** 
 * Expand current nodes's neighbors and add them to the open list.
 */
void ExpandNeighbors(const vector<int> &current, int goal[2],
    vector<vector<int>> &open, vector<vector<State>> &grid) {
  // Get current node's coordinates;
  int x1 = current[0];
  int y1 = current[1];
  int g1 = current[2];

  // Loop through current node's potential neighbors.
  for (auto d : delta) {
    int x2 = x1 + d[0];
    int y2 = y1 + d[1];

    // Check that the potential neighbor's x2 and y2 values
    // are on the grid and not closed.
    if (CheckValidCell(x2, y2, grid)) {

      // Increment g value, compute h value, and add neighbor to open list.
      int g2 = g1 + 1;
      int h2 = Heuristic(x2, y2, goal[0], goal[1]);
      AddToOpen(x2, y2, g2, h2, open, grid);
    }
  }
}

/**
 * A* search algorithm.
 */
vector<vector<State>> Search(vector<vector<State>> grid, int init[2],
                             int goal[2]) {
  // Create vector of open nodes.
  vector<vector<int>> open {};

  // Initialize the starting node.
  int x = init[0];
  int y = init[1];
  int g = 0;
  int h = Heuristic(x, y, goal[0], goal[1]);

  // Add starting node for search.
  AddToOpen(x, y ,g ,h, open, grid);

  while (!open.empty()) {

    // Sort the open list of nodes
    CellSort(&open);

    // Get the most promising node
    //auto current = open.back();
    auto current = open.back();

    // Remove it from nodes to be explored
    open.pop_back();

    // Get current node coordinates
    x = current[0];
    y = current[1];

    // Set current position on grid
    grid[x][y] = State::kPath;

    // Current position is goal position: end algorithm
    if (x == goal[0] && y == goal[1]) {
      grid[init[0]][init[1]] = State::kStart;
      grid[goal[0]][goal[1]] = State::kFinish;
      return grid;
    }

    // Current position is not yet goal position: expand search
    ExpandNeighbors(current, goal, open, grid);
  }

  // We've run out of new nodes to explore and haven't found a path.
  cout << "No path found!\n";
  return vector<vector<State>>{};
}

/**
 * Convert a State cell into its string representation.
 */
string CellString(State cell) {
  switch (cell) {
    case State::kObstacle: return "*   ";
    case State::kPath: return ".   ";
    case State::kStart: return "$   ";
    case State::kFinish: return "X   ";
    default: return "0   ";
  }
}

/**
 * Print two dimensional board to standard output.
 */
void PrintBoard(const vector<vector<State>> board) {
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      cout << CellString(board[i][j]);
    }
    cout << "\n";
  }
}

// Include test file with test functions.
//#include "test.cpp"

int main() {
  // Initial coordinates
  int init[2] = {0, 0};

  // Goal coordinates
  int goal[2] = {4, 5};

  // Read and store board from file
  auto board = ReadBoardFile("1.board");

  // Find A* 'best' path heuristic solution
  vector<vector<State>> solution = Search(board, init, goal);

  // Print solution
  PrintBoard(solution);

  //Tests
  // TestHeuristic();
  // TestAddToOpen();

}
