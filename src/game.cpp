#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "game.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define OLED_RESET_PIN -1

Game::Display::Display() : Adafruit_SSD1306(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET_PIN)
{
}

Game::Scene *currentScene = nullptr;

Game::Scene::~Scene() {}

void Game::Scene::change(Game::Scene *scene)
{
  if (currentScene != nullptr)
  {
    delete currentScene;
  }

  currentScene = scene;
}

Game::Scene *Game::Scene::current() {
  return currentScene;
}
