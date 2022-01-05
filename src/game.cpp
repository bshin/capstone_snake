#include "game.h"
#include <iostream>
#include <algorithm>
#include "SDL.h"

using std::sort;

// directional deltas
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

Game::Game(std::size_t grid_width, std::size_t grid_height, std::size_t no_pits)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)),
      pits(static_cast<int>(no_pits)) {
  
  PlaceFood();
  InitWalls(grid_width, grid_height);
}

void Game::InitWalls(int grid_width, int grid_height) {
  // initialization of wall objects
  vector<int> xVec{grid_width-20, grid_width-10, grid_width-5}; // x position of wall's starting point
  vector<int> yVec{grid_width-18, grid_width-18, grid_width-8}; // y position of wall's starting point
  vector<int> lenVec{6, 4, 5};
  vector<Wall::Orientation> oriVec{Wall::Orientation::kVert, Wall::Orientation::kHoriz, Wall::Orientation::kHoriz};
  vector<Wall::Direction> dirVec{Wall::Direction::kDown, Wall::Direction::kRight, Wall::Direction::kLeft};

  for (int i=0; i<xVec.size(); i++) {
    shared_ptr<Wall> newWall(new Wall(xVec[i], yVec[i], grid_width, grid_height, lenVec[i], dirVec[i], oriVec[i]));
    WallVec.push_back(std::move(newWall));
  }
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  PlacePit(pits); // Place pits
  
  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food, WallVec, pit);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
  
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item or wall before placing
    // food.
    if (!snake.SnakeCell(x, y) && !CellOccupied(x, y, pit)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

SDL_Point Game::GetFood() {
  return food;
}

// check if a map point is occupied by a wall
bool Game::CellOccupied(int x, int y, std::vector<SDL_Point> &obstacle){
  for(int j=0; j<obstacle.size(); j++){
    if (obstacle[j].x == x && obstacle[j].y == y){
      return true;
    }
  }
  return false;
}

// to get pit points around one pit point
const int step[3][2]{{1, 0}, {0, 1}, {1, 1}};

void Game::PlacePit(int number) {
  pit.clear();
  int i=0;
  SDL_Point point;
  int x,y;
  while(i < number){
    vector<SDL_Point> tempVec{};
    // prevent that pit is placed on head position of snake
    while(true){      
      x = random_w(engine);
      y = random_h(engine);

      point.x = x;
      point.y = y;
      tempVec.push_back(point);

      for(int j=0; j<sizeof(step)/sizeof(step[0]); j++){
          point.x = x+step[j][0];
          point.y = y+step[j][1];
          tempVec.push_back(point);
      }

      if (!CellOccupied(snake.head_x, snake.head_y, tempVec)) {
        for(int j=0; j<tempVec.size(); j++){
          pit.push_back(tempVec[j]);
        }
        break;
      }
    }
    i++;
  }
}


void Game::Update() {
  if (!snake.alive) return;

  snake.Update();
  for (int i=0; i<WallVec.size(); i++) {
    WallVec[i]->Update();
  }

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // running into wall = dead!
  for (int i=0; i<WallVec.size(); i++){
    if (CellOccupied(new_x, new_y, WallVec[i]->wall) == true) snake.alive = false;
    for (int j=0; j<snake.size-1; j++) {
      if (CellOccupied(snake.body[j].x, snake.body[j].y, WallVec[i]->wall) == true) snake.alive = false;
    }
  }
  
  // shrink body and recude score if snake enters pit
  if (CellOccupied(new_x, new_y, pit) == true){
    if (pitEntered == false) {    
      score--;
      snake.ShrinkBody();
      pitEntered = true;
    }
  }
  else pitEntered = false;

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    PlacePit(pits); // place pits at new positions every time the snake finds food
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }

}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }