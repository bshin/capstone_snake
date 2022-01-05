#include "wall.h"
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace::std;


Wall::Wall(int x, int y, int g_width, int g_height, int len, Direction dir, Orientation orient){
    grid_width = g_width;
    grid_height = g_height;
    head_x = x;
    head_y = y;
    length = len;
    direction = dir;
    orientation = orient;

    body = InitWallBody();
}

// Initialize body of moving wall
vector<SDL_Point> Wall::InitWallBody() {
  SDL_Point body_pt;
  switch (orientation){
    case Orientation::kHoriz:    
      for (int i=length-1; i>0; i--){
          if (direction == Direction::kLeft) body_pt.x = head_x + i;
          else body_pt.x = head_x - i;
          body_pt.y = head_y;
          body_pt.x = fmod(body_pt.x + grid_width, grid_width);
          body_pt.y = fmod(body_pt.y + grid_height, grid_height);
          body.push_back(body_pt);
      }
      break;
    case Orientation::kVert:
      for (int i=length-1; i>0; i--){
            body_pt.x = head_x;
            if (direction == Direction::kUp) body_pt.y = head_y + i;
            else body_pt.y = head_y - i;
            body_pt.x = fmod(body_pt.x + grid_width, grid_width);
            body_pt.y = fmod(body_pt.y + grid_height, grid_height);
            body.push_back(body_pt);
      }
      break;
    default:
      break;
    }

  return body;
}

void Wall::Update() {
  SDL_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // We first capture the head's cell before updating.
  UpdateHead();
  SDL_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // Capture the head's cell after updating.

  // Update all of the body vector items
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(prev_cell);
  }

  wall = body;
  wall.push_back(current_cell);
}

void Wall::UpdateHead() {
  switch (direction) {
    case Direction::kUp:
      head_y -= speed;
      break;

    case Direction::kDown:
      head_y += speed;
      break;

    case Direction::kLeft:
      head_x -= speed;
      break;

    case Direction::kRight:
      head_x += speed;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);
}

void Wall::UpdateBody(SDL_Point &prev_head_cell) {
  body.push_back(prev_head_cell);
  body.erase(body.begin());
}
