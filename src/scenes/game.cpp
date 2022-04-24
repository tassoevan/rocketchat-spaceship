#include "../game.h"

#define ROCKET_FLAMES_WIDTH 12
#define ROCKET_FLAMES_HEIGHT 10
#define ROCKET_WIDTH 16
#define ROCKET_HEIGHT 16
#define WARP_SPEED_FACTOR 1.5
#define SUB_WARP_SPEED_FACTOR 0.5
#define ROCKET_DX 2

Game::GameScene::GameScene() : Game::Scene(),
  rocketX(0), rocketY(0),
  warpSpeed(false), warpSpeedStartTime(-1),
  starX{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  starY{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  starSpeed{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  distance(0.0f)
{
}

inline int maxDisplacement(Game::Display &display)
{
  return (display.height() - ROCKET_HEIGHT) / 2;
}

inline int wrapSpeedBuzzerFrequency(long warpSpeedStartTime)
{
  return max(300 * exp(-1.0 * (millis() - warpSpeedStartTime) / 1000.0), 100);
}

void Game::GameScene::update(Game::Display &display)
{
  bool upPressed = digitalRead(GAME_UP_BUTTON_PIN) == LOW;
  bool downPressed = digitalRead(GAME_DOWN_BUTTON_PIN) == LOW;
  bool firePressed = digitalRead(GAME_FIRE_BUTTON_PIN) == LOW;

  if (upPressed && rocketY < maxDisplacement(display))
  {
    rocketY++;
  }

  if (downPressed && rocketY > -maxDisplacement(display))
  {
    rocketY--;
  }

  warpSpeed = firePressed;

  if (!warpSpeed) {
    warpSpeedStartTime = -1;
    rocketX = max(0, rocketX - SUB_WARP_SPEED_FACTOR * ROCKET_DX);
    noTone(GAME_BUZZER_PIN);
    return;
  }

  rocketX = min(rocketX + WARP_SPEED_FACTOR * ROCKET_DX, (display.width() - (ROCKET_FLAMES_WIDTH + 1 + ROCKET_WIDTH)) / 2);

  if (warpSpeedStartTime == -1) {
    warpSpeedStartTime = millis();
  }

  tone(GAME_BUZZER_PIN, wrapSpeedBuzzerFrequency(warpSpeedStartTime));
}

// 'rocket.png', 16x16px
const uint8_t rocketBitmap[] PROGMEM = {
    0x00, 0x00, 0xf0, 0x00, 0x7d, 0x80, 0x7f, 0xf8, 0x78, 0x1c, 0x60, 0x06, 0xc0, 0x03, 0xcd, 0xb3,
    0xcd, 0xb3, 0xc0, 0x03, 0x60, 0x06, 0x60, 0x0c, 0x67, 0xf8, 0x7f, 0xc0, 0xf8, 0x00, 0x00, 0x00};

// 'rocket_flames.png', 12x20px
const uint8_t rocketFlamesBitmap[] PROGMEM = {
    0x00, 0x00, 0x03, 0xc0, 0x1f, 0x60, 0x7a, 0xb0, 0xd5, 0x50, 0xca, 0x90, 0x71, 0x70, 0x1e, 0xe0,
    0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x80, 0x1d, 0xe0, 0x3a, 0xb0, 0x35, 0x70,
    0x1e, 0xe0, 0x07, 0x80, 0x20, 0x00, 0x00, 0x00};

void Game::GameScene::drawRocket(Game::Display &display, int x, int y)
{
  static uint8_t flameFrame = 0;
  const uint8_t *flameBitmap = flameFrame % 2 == 0 ? rocketFlamesBitmap : (rocketFlamesBitmap + 20);

  display.drawBitmap(x + ROCKET_FLAMES_WIDTH + 1, y, rocketBitmap, ROCKET_WIDTH, ROCKET_HEIGHT, WHITE);
  display.drawBitmap(x, y + (ROCKET_HEIGHT - ROCKET_FLAMES_HEIGHT) / 2, flameBitmap, ROCKET_FLAMES_WIDTH, ROCKET_FLAMES_HEIGHT, WHITE);

  flameFrame++;
}

void Game::GameScene::drawStars(Game::Display &display)
{
  for (int i = 0; i < Game::GameScene::STAR_COUNT; ++i) {
    if (starX[i] == -1 || starY[i] == -1 || starSpeed[i] == -1) {
      starX[i] = random(display.width());
      starY[i] = random(display.height());
      starSpeed[i] = random(5) + 1;
    }

    starX[i] -= (warpSpeed ? 10 : 1) * starSpeed[i];
    if (starX[i] < 0) {
      starX[i] = display.width();
    }

    if (warpSpeed) {
      display.drawLine(starX[i], starY[i], starX[i] - starSpeed[i], starY[i], WHITE);
    } else {
      display.drawPixel(starX[i], starY[i], WHITE);
    }
  }
}

#define SPEED_OF_LIGHT 299792458L
#define ASTRONOMICAL_UNIT 149597870700L

void Game::GameScene::render(Game::Display &display)
{
  drawStars(display);
  drawRocket(display, rocketX, maxDisplacement(display) - rocketY);

  display.setTextSize(1);
  display.setTextColor(WHITE);

  if (warpSpeed) {
    auto text = F("WARP SPEED");
    int16_t textX, textY;
    uint16_t textWidth, textHeight;
    display.getTextBounds(text, 0, 0, &textX, &textY, &textWidth, &textHeight);
    display.setCursor(display.width() - textWidth, display.height() - textHeight);
    display.print(text);
  }

  float speed = SPEED_OF_LIGHT * (warpSpeed ? 1 : 0.5);
  distance += speed * (1.0 / 24);
  float score = distance / ASTRONOMICAL_UNIT;

  display.setCursor(0, 0);
  display.print(String(score, 2) + " UA");
}
