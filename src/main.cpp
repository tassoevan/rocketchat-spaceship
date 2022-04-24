#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "game.h"

Game::Display display;

void setup() {
  Game::Scene::change(new Game::LogoScene());

  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  pinMode(GAME_UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(GAME_DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(GAME_FIRE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(GAME_BUZZER_PIN, OUTPUT);
}

void loop() {
  Game::Scene::current()->update(display);

  display.clearDisplay();
  Game::Scene::current()->render(display);
  display.display();

  delay(1000 / 24);
}
