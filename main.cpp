#include <Arduino.h>
#include <TFT_eSPI.h>  // Display library for T-PicoC3
#include <IRremote.hpp> // IR control library

// Pin definitions for T-PicoC3
#define IR_LED_PIN 10      // GPIO for IR LED
#define BUTTON_OK 3        // Run button (OK)
#define BUTTON_LEFT 7      // IO7 (Power when held) / Left
#define BUTTON_RIGHT 6     // IO6 (Right)
#define BATTERY_PIN 4      // ADC pin for battery monitoring

// TFT Display
TFT_eSPI tft = TFT_eSPI();

// Function prototypes
void setupDisplay();
void handleButtons();
void sendIRSignal();
float getBatteryVoltage();

void setup() {
    Serial.begin(115200);
    
    // Initialize display
    setupDisplay();
    
    // Initialize buttons
    pinMode(BUTTON_OK, INPUT_PULLUP);
    pinMode(BUTTON_LEFT, INPUT_PULLUP);
    pinMode(BUTTON_RIGHT, INPUT_PULLUP);
    
    // Initialize IR LED
    IrSender.begin(IR_LED_PIN, ENABLE_LED_FEEDBACK);
    
    // Display startup screen
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Bruce Firmware", 40, 20, 2);
    tft.drawString("T-PicoC3 Version", 30, 50, 2);
    delay(2000);
}

void loop() {
    handleButtons();
}

// Function to initialize the display
void setupDisplay() {
    tft.init();
    tft.setRotation(1);  // Adjust as needed
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);
}

// Function to check button inputs
void handleButtons() {
    if (digitalRead(BUTTON_OK) == LOW) {
        tft.fillScreen(TFT_BLACK);
        tft.drawString("OK Pressed", 60, 60, 2);
        delay(500);
    }

    if (digitalRead(BUTTON_LEFT) == LOW) {
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Left Pressed", 60, 60, 2);
        delay(500);
    }

    if (digitalRead(BUTTON_RIGHT) == LOW) {
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Right Pressed", 60, 60, 2);
        delay(500);
    }
}

// Function to read battery voltage
float getBatteryVoltage() {
    int rawValue = analogRead(BATTERY_PIN);
    return (rawValue / 4095.0) * 3.3 * 2;  // Assuming a voltage divider
}
// TV power-off codes (replace or add more if needed)
const uint32_t FOX_POWER_OFF = 0x19E6;      // Fox TV power-off code (16-bit NEC)
const uint32_t VOX_POWER_OFF = 0x20DF10EF;  // Example Vox TV power-off code (32-bit NEC, placeholder)
const uint32_t SONY_POWER_OFF = 0xA90;      // Sony TV power-off code (12-bit Sony)
const uint32_t SAMSUNG_POWER_OFF = 0xE0E040BF;  // Samsung TV power-off code (32-bit NEC)

// Function to send TV power-off IR signals for multiple brands
void sendIRSignals() {
    tft.fillScreen(TFT_BLACK);
    tft.drawString("Sending TV OFF", 60, 50, 2);
    
    // Send power-off codes for various brands
    tft.drawString("Fox TV...", 60, 70, 2);
    IrSender.sendNEC(FOX_POWER_OFF, 16);  // 16-bit NEC for Fox TV
    delay(1000);

    tft.drawString("Vox TV...", 60, 90, 2);
    IrSender.sendNEC(VOX_POWER_OFF, 32);  // 32-bit NEC for Vox TV
    delay(1000);

    tft.drawString("Sony TV...", 60, 110, 2);
    IrSender.sendSony(SONY_POWER_OFF, 12);  // 12-bit Sony code
    delay(1000);

    tft.drawString("Samsung TV...", 60, 130, 2);
    IrSender.sendNEC(SAMSUNG_POWER_OFF, 32);  // 32-bit NEC for Samsung
    delay(1000);

    tft.fillScreen(TFT_BLACK);
    tft.drawString("TVs Turned OFF!", 60, 50, 2);
    delay(2000);
}

// Update button handler to trigger IR transmission
void handleButtons() {
    if (digitalRead(BUTTON_OK) == LOW) {
        sendIRSignals();
    }

    if (digitalRead(BUTTON_LEFT) == LOW) {
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Left Pressed", 60, 60, 2);
        delay(500);
    }

    if (digitalRead(BUTTON_RIGHT) == LOW) {
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Right Pressed", 60, 60, 2);
        delay(500);
    }
}
// Function to read battery voltage and display it on the screen
void displayBatteryStatus() {
    int rawValue = analogRead(BATTERY_PIN);
    float voltage = (rawValue / 4095.0) * 3.3 * 2;  // Convert ADC value to voltage
    
    tft.fillScreen(TFT_BLACK);
    tft.drawString("Battery: " + String(voltage, 2) + "V", 60, 20, 2);

    if (voltage < 3.3) {
        tft.setTextColor(TFT_RED);
        tft.drawString("Low Battery!", 60, 50, 2);
    } else {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("Battery OK", 60, 50, 2);
    }
    tft.setTextColor(TFT_WHITE);
    delay(2000);
}

// Function to optimize power consumption
void enterLowPowerMode() {
    tft.fillScreen(TFT_BLACK);
    tft.drawString("Entering Sleep...", 60, 50, 2);
    delay(1000);

    tft.writecommand(TFT_DISPOFF);  // Turn off display
    pinMode(IR_LED_PIN, INPUT);     // Disable IR LED
    esp_deep_sleep_start();         // Enter deep sleep mode
}

// Modify button handler to include power-off when holding the left button
void handleButtons() {
    if (digitalRead(BUTTON_OK) == LOW) {
        sendIRSignals();
    }

    if (digitalRead(BUTTON_LEFT) == LOW) {
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Left Pressed", 60, 60, 2);
        delay(500);
    }

    if (digitalRead(BUTTON_RIGHT) == LOW) {
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Right Pressed", 60, 60, 2);
        delay(500);
    }

    if (digitalRead(BUTTON_LEFT) == LOW && millis() > 3000) {  // Hold left button to power off
        enterLowPowerMode();
    }
}
// Menu options for TV brands
const char* menuItems[] = {"All TVs", "Fox TV", "Vox TV", "Sony TV", "Samsung TV"};
const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);

int currentSelection = 0;  // Track current menu position

// Function to display the menu
void displayMenu() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);

    for (int i = 0; i < menuSize; i++) {
        if (i == currentSelection) {
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.drawString(menuItems[i], 40, 30 + (i * 20), 2);
    }
}

// Function to handle menu selection
void selectMenuItem() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.drawString("Sending " + String(menuItems[currentSelection]), 40, 50, 2);

    switch (currentSelection) {
        case 0:  // All TVs
            sendIRSignals();
            break;
        case 1:  // Fox TV
            IrSender.sendNEC(FOX_POWER_OFF, 16);
            break;
        case 2:  // Vox TV
            IrSender.sendNEC(VOX_POWER_OFF, 32);
            break;
        case 3:  // Sony TV
            IrSender.sendSony(SONY_POWER_OFF, 12);
            break;
        case 4:  // Samsung TV
            IrSender.sendNEC(SAMSUNG_POWER_OFF, 32);
            break;
    }
    
    delay(2000);
    displayMenu();  // Return to menu after sending
}

// Modify button handler for menu navigation
void handleButtons() {
    if (digitalRead(BUTTON_LEFT) == LOW) {
        currentSelection = (currentSelection - 1 + menuSize) % menuSize;  // Navigate up
        displayMenu();
        delay(300);
    }

    if (digitalRead(BUTTON_RIGHT) == LOW) {
        currentSelection = (currentSelection + 1) % menuSize;  // Navigate down
        displayMenu();
        delay(300);
    }

    if (digitalRead(BUTTON_OK) == LOW) {
        selectMenuItem();  // Confirm selection and send IR signal
    }

    if (digitalRead(BUTTON_LEFT) == LOW && millis() > 3000) {  // Hold to power off
        enterLowPowerMode();
    }
}

