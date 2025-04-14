
#include <M5StickCPlus2.h>
#include <FastLED.h>

#define LED_PIN     32
#define NUM_LEDS    8
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

int r = 0, g = 0, b = 0;
int sliderIndex = 0; // 0 = R, 1 = G, 2 = B

unsigned long lastPressTime = 0;
bool buttonHeld = false;

void setup() {
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    drawUI();
    updateLEDs();
}

void loop() {
    M5.update();

    if (M5.BtnA.pressedFor(500)) {
        if (!buttonHeld) {
            sliderIndex = (sliderIndex + 1) % 3;
            drawUI();
            buttonHeld = true;
        }
    } else if (M5.BtnA.wasPressed()) {
        incrementSlider();
        drawUI();
        updateLEDs();
        buttonHeld = false;
    }

    if (M5.BtnB.wasPressed()) {
        decrementSlider();
        drawUI();
        updateLEDs();
    }

    delay(50);
}

void incrementSlider() {
    switch (sliderIndex) {
        case 0: r = min(255, r + 5); break;
        case 1: g = min(255, g + 5); break;
        case 2: b = min(255, b + 5); break;
    }
}

void decrementSlider() {
    switch (sliderIndex) {
        case 0: r = max(0, r - 5); break;
        case 1: g = max(0, g - 5); break;
        case 2: b = max(0, b - 5); break;
    }
}

void updateLEDs() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(r, g, b);
    }
    FastLED.show();
}

void drawSlider(int y, const char* label, int value, uint16_t color, bool selected) {
    int barWidth = map(value, 0, 255, 0, 200);
    M5.Lcd.setTextColor(color, BLACK);
    M5.Lcd.setCursor(10, y);
    M5.Lcd.printf("%s: %3d", label, value);
    M5.Lcd.fillRect(80, y, 200, 10, selected ? color : DARKGREY);
    M5.Lcd.fillRect(80, y, barWidth, 10, color);
}

void drawUI() {
    M5.Lcd.fillScreen(BLACK);
    drawSlider(30, "R", r, RED, sliderIndex == 0);
    drawSlider(60, "G", g, GREEN, sliderIndex == 1);
    drawSlider(90, "B", b, BLUE, sliderIndex == 2);
}
