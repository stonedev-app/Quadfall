#include <Arduboy2.h>
#include "game.h"
#include "input.h"
#include "renderer.h"

Arduboy2 arduboy;

void setup() {
    arduboy.begin();
    arduboy.setFrameRate(60);
    arduboy.clear();
    gameInit();
}

void loop() {
    if (!arduboy.nextFrame()) return;

    arduboy.pollButtons();
    inputUpdate(arduboy);
    gameUpdate();
    renderFrame(arduboy);
}
