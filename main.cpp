#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cout;
using std::ifstream;
using std::istringstream;
using std::sort;
using std::string;
using std::vector;

enum class State { kEmpty, kObstacle, kClosed, kPath, kStart, kFinish };

/**
 * @brief Represents a Node with x,y coordinates, g cost value and
 * h heuristic value.
 */
struct Node {
  int x;
  int y;
  int g;
  int h;
};

/**
 * Check if a given cell is an allowed move.
 *
 * @param x x coordinate
 * @param y y coordinate
 * @param grid search grid
 *
 * @return whether a cell is a valid position
 */
bool CheckValidCell(int x, int y, vector<vector<State>> grid) {
  if (x < 0 or y < 0)
    return false;

  if (x >= grid.size() || y >= grid[0].size())
    return false;

  if (grid[x][y] == State::kEmpty)
    return true;
  else
    return false;
}

/**
 * Add a node to the open list and mark it as open.
 *
 * @param x node's x coordinate
 * @param y node's y coordinate
 * @param g node's accumulated cost
 * @param h node's heuristic value
 */
void AddToOpen(Node n, vector<Node> &open, vector<vector<State>> &grid) {
  // Add node to open vector, and mark grid cell as closed.
  open.push_back(n);
  grid[n.x][n.y] = State::kClosed;
}

/**
 * Compare the f-values of two nodes.
 *
 * @param a first node
 * @param b second node
 *
 * @return true if first node has a f-value greater than the second,
 * false otherwise.
 */
bool Compare(Node a, Node b) { return ((a.g + a.h) > (b.g + b.h)); }

/**
 * Sort inplace a two-dimensional vector of ints in descending order.
 *
 * @param v vector of nodes to be sorted
 */
void CellSort(vector<Node> *v) { sort(v->begin(), v->end(), Compare); }

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

/**
 * Read a 2-dimensional 0-1 board from a file path to a grid of State states.
 *
 * @param path path to file to be read
 *
 * @return grid of State cells
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
 * Calculate the Manhattan Distance between two points.
 *
 * @param x1 current x coordinate
 * @param y1 current y coordinate
 * @param x2 goal x coordinate
 * @param y2 goal y coordinate
 *
 * @return the heuristic value for x,y coordinates
 */
int Heuristic(int x1, int y1, int x2, int y2) {
  return std::abs(x2 - x1) + std::abs(y2 - y1);
}

/**
 * Expand current nodes's neighbors and add them to the open list.
 *
 * @param current current node
 * @param goal goal node x,y coordinates
 * @param open vector of nodes to be searched
 * @param grid two dimensional grid of nodes
 */
void ExpandNeighbors(const Node &current, int goal[2], vector<Node> &open,
                     vector<vector<State>> &grid) {
  // Get current node's coordinates;
  int x1 = current.x;
  int y1 = current.y;
  int g1 = current.g;

  // directional deltas, i.e. possible movement directions
  const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

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
      AddToOpen(Node{x2, y2, g2, h2}, open, grid);
    }
  }
}

/**
 * A* search algorithm.
 *
 * @param grid 2-dimensional grid
 * @param init initial x,y coordinates
 * @param goal goal x,y coordinates
 *
 * @return 2-dimensional grid with search path
 */
vector<vector<State>> Search(vector<vector<State>> grid, int init[2],
                             int goal[2]) {
  // Create vector of open nodes.
  vector<Node> open{};

  // Initialize the starting node.
  int x = init[0];
  int y = init[1];
  int g = 0;
  int h = Heuristic(x, y, goal[0], goal[1]);

  // Add starting node to possible search nodes.
  AddToOpen(Node{x, y, g, h}, open, grid);

  while (!open.empty()) {

    // Sort nodes to be searched by heuristic value
    CellSort(&open);

    // Get most promising node and remove it from open nodes
    auto current = open.back();
    open.pop_back();

    // Get current node coordinates
    x = current.x;
    y = current.y;

    // Set node's coordinates as path
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
  case State::kObstacle:
    return "*   ";
  case State::kPath:
    return ".   ";
  case State::kStart:
    return "$   ";
  case State::kFinish:
    return "X   ";
  default:
    return "0   ";
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
}
