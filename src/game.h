#ifndef GAME_H
#define GAME_H

#include <random>
#include <vector>
#include <memory>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "wall.h"

using namespace::std;

enum class State {kEmpty, kObstacle, kClosed, kPath, kGoal, kStart};

class Game {
 public:

  Game(std::size_t grid_width, std::size_t grid_height, std::size_t no_pits);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  SDL_Point GetFood();
  vector<shared_ptr<Wall>> WallVec; // smart pointers to wall objects

 private:
  Snake snake;
  SDL_Point food;
  vector<SDL_Point> wall;
  vector<SDL_Point> pit;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void PlaceFood();
  void PlacePit(int number);
  void InitWalls(int grid_width, int grid_height);
  void Update();
  bool CellOccupied(int x, int y, std::vector<SDL_Point> &obstacle);
  bool pitEntered{false};
  int pits;
};

#endif