
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

enum class State { kEmpty, kObstacle, kClosed, kPath };

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
  AddToOpen(x, y ,g ,h, open, grid);

  while (!open.empty()) {

    // Sort the open list of nodes
    CellSort(&open);

    // Get the most promising node
    auto current = open.back();

    // Remove it from nodes to be explored
    open.pop_back();

    // Get current node coordinates
    x = current[0];
    y = current[1];

    // Set current position on grid
    grid[x][y] = State::kPath;

    // Current position is goal position: end algorithm
    if (x == goal[0] && y == goal[1]) return grid;

    // Current position is not yet goal position: expand search
    // ExpandNeighbors


  // We've run out of new nodes to explore and haven't found a path.
  cout << "No path found!\n";
  return vector<vector<State>>{};
}

string CellString(State cell) {
  switch (cell) {
    case State::kObstacle: return "*   ";
    case State::kPath: return ".   ";
    default: return "0   ";
  }
}

void PrintBoard(const vector<vector<State>> board) {
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      cout << CellString(board[i][j]);
    }
    cout << "\n";
  }
}

#include "test.cpp"

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
  TestHeuristic();
  TestAddToOpen();

}
