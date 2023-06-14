#include "Engine.h"
#include <stdlib.h>
#include <memory.h>
//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

constexpr int BRICK_WIDTH = 60;
constexpr int BRICK_HEIGHT = 20;
constexpr int BRICK_SPACING = 6;
constexpr int BRICK_ROWS = 5;
constexpr int BRICK_COLUMNS = 15;

constexpr int BONUS_WIDTH = 15;
constexpr int BONUS_HEIGHT = 15;
constexpr float BONUS_SPEED = 100.f;

// Define an array to store the state of each brick
bool bricks[BRICK_ROWS][BRICK_COLUMNS];

constexpr int PADDLE_WIDTH = 150;
constexpr int PADDLE_HEIGHT = 20;
constexpr float PADDLE_SPEED = 600.f;

constexpr int BALL_RADIUS = 5;
constexpr float BALL_SPEED = 300.f;

float paddleX = 0;

float ballX = 0;
float ballY = 0;
int ballDX = 1;
int ballDY = 1;

int startX = 0;
int startY = 0;
int totalBricksHeight = BRICK_HEIGHT*BRICK_ROWS;
int totalBricksWidth = BRICK_WIDTH*BRICK_COLUMNS;
int totalSpacingHeight = (BRICK_ROWS - 1) * BRICK_SPACING;
int totalSpacingWidth = (BRICK_COLUMNS - 1)* BRICK_SPACING;
int totalHeight = totalBricksHeight + totalSpacingHeight;
int totalWidth = totalBricksWidth + totalSpacingWidth;

float bonusX = 0.f;
float bonusY = 0.f;
bool bonusActive= false;
bool triggered = false;
// initialize game data in this function
void initialize()
{
    paddleX = (SCREEN_WIDTH - PADDLE_WIDTH) / 2;
    ballX = (SCREEN_WIDTH - BALL_RADIUS * 2) / 2;
    ballY = (SCREEN_HEIGHT - BALL_RADIUS * 2) / 2;

    for (auto& brick : bricks)
    {
        for (bool& col : brick)
        {
        col = true;
        }
    }

    startX = (SCREEN_WIDTH - totalWidth) / 2;
    startY = (SCREEN_WIDTH - totalHeight) / 40;
    
    bonusActive = false;
}

void Bonus(float &dt)
{
    
    if (!bonusActive)
    {
        
        if (!triggered)
        {
            //bonusActive = true;
            triggered = true;
            bonusX = static_cast<float>(rand() % (SCREEN_WIDTH - BONUS_WIDTH));
            bonusY = 0.0f;
        }
    }
    else
    {
        bonusY += BONUS_SPEED * dt;

        // Collision detection with paddle
        if (bonusY + BONUS_HEIGHT >= SCREEN_HEIGHT - PADDLE_HEIGHT && bonusX + BONUS_WIDTH >= paddleX && bonusX <= paddleX + PADDLE_WIDTH) {
            // Bonus caught by the paddle
            bonusActive = false;
            triggered = false;
            // Implement bonus effect here
            //PADDLE_WIDTH += 10;
        }

        // Collision detection with bottom screen boundary
        if (bonusY + BONUS_HEIGHT >= SCREEN_HEIGHT) {
            // Bonus missed
            bonusActive = false;
            triggered = false;
        }
    }
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
      if (is_window_active())
      {
          if (is_key_pressed(VK_LEFT) && paddleX > 0) {
              paddleX -= PADDLE_SPEED*dt;
          }
          if (is_key_pressed(VK_RIGHT) && paddleX < SCREEN_WIDTH - PADDLE_WIDTH) {
              paddleX += PADDLE_SPEED*dt;
          }
          ballX = ballX + static_cast<float>(ballDX) * BALL_SPEED * dt;
          ballY = ballY + static_cast<float>(ballDY) * BALL_SPEED * dt;

          if (ballX < 0 || ballX > SCREEN_WIDTH - BALL_RADIUS * 2) {
              ballDX = -ballDX;
          }
          if (ballY < 0 || ballY > SCREEN_HEIGHT - BALL_RADIUS * 2) {
              ballDY = -ballDY;
          }

          if (ballY + BALL_RADIUS * 2 >= SCREEN_HEIGHT - PADDLE_HEIGHT && ballX + BALL_RADIUS * 2 >= paddleX && ballX <= paddleX + PADDLE_WIDTH) {
              ballDY = -ballDY;
          }

          if (ballY + BALL_RADIUS * 2 >= SCREEN_HEIGHT) {
              schedule_quit_game();
          }

          for (int row = 0; row < BRICK_ROWS; ++row) {
              for (int col = 0; col < BRICK_COLUMNS; ++col) {
                  if (bricks[row][col]) {
                      const int brickX = startX + col * (BRICK_WIDTH + BRICK_SPACING);
                      const int brickY = startY + row * (BRICK_HEIGHT + BRICK_SPACING);

                      if (ballY <= brickY + BRICK_HEIGHT && ballY + BALL_RADIUS * 2 >= brickY &&
                          ballX <= brickX + BRICK_WIDTH && ballX + BALL_RADIUS * 2 >= brickX)
                      {
                          // Collision with brick detected
                          ballDY = -ballDY;
                          bricks[row][col] = false; // Mark the brick as destroyed
                          if (rand() % 2 == 0)
                          {
                              bonusActive = true;
                          }
                      }
                  }
              }
          }
          
          Bonus(dt);
          
          if (is_key_pressed(VK_ESCAPE))
              schedule_quit_game();
      }
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
  // clear backbuffer
  memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

  clear_buffer();

  // Draw the paddle
  for (int x = static_cast<int>(paddleX); x < static_cast<int>(paddleX + PADDLE_WIDTH); ++x) {
    for (int y = SCREEN_HEIGHT - PADDLE_HEIGHT; y < SCREEN_HEIGHT; ++y) {
      buffer[y][x] = 0xFF0000;  // Set paddle color to red
    }
  }

  // Draw the ball
  for (int x = static_cast<int>(ballX); x < static_cast<int>(ballX + BALL_RADIUS * 2); ++x) {
    for (int y = static_cast<int>(ballY); y < static_cast<int>(ballY + BALL_RADIUS * 2); ++y) {
      buffer[y][x] = 0xFFFFFF;  // Set ball color to white
    }
  }

  // BRICKS 
  for (int row = 0; row < BRICK_ROWS; ++row) {
    for (int col = 0; col < BRICK_COLUMNS; ++col) {
      if (bricks[row][col]) {
        const int brickX = startX + col * (BRICK_WIDTH + BRICK_SPACING);
        const int brickY = startY + row * (BRICK_HEIGHT + BRICK_SPACING);

        // Draw the brick
        for (int x = brickX; x < brickX + BRICK_WIDTH; ++x) {
          for (int y = brickY; y < brickY + BRICK_HEIGHT; ++y) {
            buffer[y][x] = 0x00FF00;  // Set brick color to green
          }
        }
      }
    }
  }

  if (bonusActive) {
      for (int x = static_cast<int>(bonusX); x < static_cast<int>(bonusX + BONUS_WIDTH); ++x) {
          for (int y = static_cast<int>(bonusY); y < static_cast<int>(bonusY + BONUS_HEIGHT); ++y) {
              buffer[y][x] = 0xFFFF00;  // Set bonus color to yellow
          }
      }
  }

}

// free game data in this function
void finalize()
{
}

