#pragma once

#include <Adafruit_SSD1306.h>

#define GAME_UP_BUTTON_PIN 0
#define GAME_DOWN_BUTTON_PIN 1
#define GAME_FIRE_BUTTON_PIN 2
#define GAME_BUZZER_PIN 3

namespace Game {
  class Display: public Adafruit_SSD1306 {
    public:
      Display();
  };

  class Scene {
    public:
      virtual ~Scene();
      virtual void update(Display& display) = 0;
      virtual void render(Display& display) = 0;
      static void change(Scene *scene);
      static Scene *current();
  };

  class LogoScene: public Game::Scene {
    public:
      virtual void update(Display& display);
      virtual void render(Display& display);
  };

  class GameScene: public Game::Scene {
    public:
      GameScene();
      virtual void update(Display& display);
      virtual void render(Display& display);
    private:
      int rocketX;
      int rocketY;
      bool warpSpeed;
      long warpSpeedStartTime;
      static const int STAR_COUNT = 20;
      int starX[STAR_COUNT];
      int starY[STAR_COUNT];
      int starSpeed[STAR_COUNT];
      float distance;
      void drawRocket(Game::Display& display, int x, int y);
      void drawStars(Game::Display& display);
  };
}
