#ifndef WALL_H
#define WALL_H

#include <vector>
#include "SDL.h"

using namespace::std;

class Wall {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };
  enum class Orientation { kVert, kHoriz };

  Wall(int head_x, int head_y, int grid_width, int grid_height, int length, Direction direction, Orientation orientation);

  void Update();  
  vector<SDL_Point> wall;

  Direction direction;
  Orientation orientation;

  float speed{0.1f};
  int size{1};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;
  int length;

 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &prev_cell);
  vector<SDL_Point> InitWallBody();
  int grid_width;
  int grid_height;
};

#endif