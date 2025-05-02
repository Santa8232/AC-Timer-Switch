#include <Arduino.h>
#include <U8g2lib.h>
#include <EEPROM.h>
#include <OneButton.h>

// --- OLED Display ---
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// --- Pin Definitions ---
const uint8_t TRIAC_PIN = 3;
const uint8_t BUTTON_PIN = 2;

// --- Timing & State ---
unsigned long previousMillis = 0;
bool triacState = false;
unsigned long onTimeMillis = 150000;    // 2 min 30 sec
unsigned long offTimeMillis = 300000;   // 5 min
unsigned long timeRemaining = 0;

// --- EEPROM Addresses ---
const int EEPROM_ON_ADDR = 0;
const int EEPROM_OFF_ADDR = 4;

// --- Edit Mode ---
bool editMode = false;
bool editingHours = true;
bool editingOnTime = true;

const unsigned long MAX_TIME_MILLIS = 3600000; // 1 hour
const unsigned long MIN_TIME_MILLIS = 60000;   // 1 minute

// --- Button Setup ---
OneButton button(BUTTON_PIN, true);  // active LOW

// --- Inactivity Timer ---
unsigned long lastEditTime = 0;
const unsigned long EDIT_TIMEOUT = 10000; // 10 seconds

// --- Format Time ---
String formatTime(unsigned long ms) {
  unsigned long totalSec = ms / 1000;
  unsigned int minutes = (totalSec / 60) % 60;
  unsigned int hours = totalSec / 3600;
  return (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
}

String formatSeconds(unsigned long ms) {
  return String(ms / 1000);
}

//Display icons
static const unsigned char ON[] U8X8_PROGMEM = {0x04,0x10,0xc4,0x31,0xf0,0x07,0xf8,0x0f,0xf8,0x0f,0xfb,0x6f,0xf8,0x0f,0xf8,0x0f,0xf4,0x17,0xe4,0x33,0xe0,0x03,0xe0,0x03,0xc0,0x01,0xc0,0x01,0x00,0x00};

static const unsigned char OFF[] U8X8_PROGMEM = {0x00,0x00,0xc0,0x01,0x70,0x07,0x18,0x0c,0x08,0x08,0x0c,0x18,0x0c,0x18,0x0c,0x18,0x08,0x08,0x10,0x0c,0xf0,0x07,0xe0,0x03,0x60,0x03,0xc0,0x01,0x00,0x00};
void draw(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    u8g2.drawXBMP(57, 3, 15, 16, ON);

    u8g2.sendBuffer();
}


// --- EEPROM Read/Write ---
void saveOnTime(unsigned long t) { EEPROM.put(EEPROM_ON_ADDR, t); }
void saveOffTime(unsigned long t) { EEPROM.put(EEPROM_OFF_ADDR, t); }
unsigned long readOnTime() {
  unsigned long t;
  EEPROM.get(EEPROM_ON_ADDR, t);
  return constrain(t, MIN_TIME_MILLIS, MAX_TIME_MILLIS);
}
unsigned long readOffTime() {
  unsigned long t;
  EEPROM.get(EEPROM_OFF_ADDR, t);
  return constrain(t, MIN_TIME_MILLIS, MAX_TIME_MILLIS);
}

//// --- Display ---
//void displayState() {
//  u8g2.clearBuffer();
//  u8g2.setFont(u8g2_font_timR18_tr);
//
//  // ON time
//  u8g2.drawStr(4, 39, formatTime(onTimeMillis).c_str());
//  if (editMode && editingOnTime && editingHours) {
//    u8g2.drawLine(6, 43, 26, 43);  // underline HH
//  }
//  if (editMode && editingOnTime && !editingHours) {
//    u8g2.drawLine(35, 43, 55, 43); // underline MM
//  }
//
//  // OFF time
//  u8g2.drawStr(68, 39, formatTime(offTimeMillis).c_str());
//  if (editMode && !editingOnTime && editingHours) {
//    u8g2.drawLine(69, 43, 89, 43); // underline HH
//  }
//  if (editMode && !editingOnTime && !editingHours) {
//    u8g2.drawLine(99, 43, 119, 43); // underline MM
//  }
//
//  // Countdown seconds
//  u8g2.setFont(u8g2_font_timR14_tr);
//  u8g2.drawStr(45, 63, formatSeconds(timeRemaining).c_str());
//
//  // Edit mode label
//  if (editMode) {
//    u8g2.setFont(u8g2_font_6x10_tr);
//    u8g2.drawStr(45, 10, "EDIT MODE");
//  }
//
//  u8g2.sendBuffer();
//}

void displayState() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_timR18_tr);

  // ON time
  u8g2.drawStr(4, 39, formatTime(onTimeMillis).c_str());
  if (editMode && editingOnTime && editingHours) {
    u8g2.drawLine(6, 43, 26, 43);  // underline HH
  }
  if (editMode && editingOnTime && !editingHours) {
    u8g2.drawLine(35, 43, 55, 43); // underline MM
  }

  // OFF time
  u8g2.drawStr(68, 39, formatTime(offTimeMillis).c_str());
  if (editMode && !editingOnTime && editingHours) {
    u8g2.drawLine(69, 43, 89, 43); // underline HH
  }
  if (editMode && !editingOnTime && !editingHours) {
    u8g2.drawLine(99, 43, 119, 43); // underline MM
  }

  // Countdown seconds
  u8g2.setFont(u8g2_font_timR14_tr);
String secondsText = formatSeconds(timeRemaining) + " sec";
u8g2.drawStr(45, 63, secondsText.c_str());

  // Edit mode label
  if (editMode) {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(45, 10, "EDIT MODE");
  }

  // Relay status
  u8g2.setFont(u8g2_font_6x10_tr);
//  u8g2.drawStr(0, 10, triacState ? "OFF" : "ON");

   u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    u8g2.drawXBMP(57, 3, 15, 16,triacState ? OFF : ON);

  u8g2.sendBuffer();
}


// --- Button Events ---
void onShortClick() {
  if (!editMode) return;

  lastEditTime = millis();

  if (editingOnTime) {
    unsigned int h = (onTimeMillis / 3600000) % 24;
    unsigned int m = (onTimeMillis / 60000) % 60;
    if (editingHours) h = (h + 1) % 24;
    else m = (m + 1) % 60;
    onTimeMillis = h * 3600000UL + m * 60000UL;
    saveOnTime(onTimeMillis);
  } else {
    unsigned int h = (offTimeMillis / 3600000) % 24;
    unsigned int m = (offTimeMillis / 60000) % 60;
    if (editingHours) h = (h + 1) % 24;
    else m = (m + 1) % 60;
    offTimeMillis = h * 3600000UL + m * 60000UL;
    saveOffTime(offTimeMillis);
  }

  displayState();
}

void onLongPressStart() {
  lastEditTime = millis();

  if (!editMode) {
    editMode = true;         // Enter edit mode
    editingOnTime = true;    // Focus on ON time field
    editingHours = true;     // Start with the hours field
  }

  displayState();            // Update the OLED display
}

void onLongPressStop() {
  lastEditTime = millis();

  if (editMode) {
    if (editingHours) {
      editingHours = false; // Switch to minutes
    }
    else if (editingOnTime) {
      editingOnTime = false; // Switch to OFF time
      editingHours = true;
    }
    else {
      editingOnTime = true;
      editingHours = true;
    }
    displayState();
  }
}                                                                                                 
// --- Relay Control ---
void updateRelay() {
  unsigned long now = millis();
  unsigned long interval = triacState ? onTimeMillis : offTimeMillis;

  if (!editMode) {
    if (now - previousMillis >= interval) {
      triacState = !triacState;
      digitalWrite(TRIAC_PIN, triacState ? HIGH : LOW);
      previousMillis = now;
      timeRemaining = interval;
    } else {
      timeRemaining = interval - (now - previousMillis);
    }
  }
}

// --- Setup ---
void setup() {
  pinMode(TRIAC_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(TRIAC_PIN, LOW);

  u8g2.begin();

  onTimeMillis = readOnTime();
  offTimeMillis = readOffTime();

  button.attachClick(onShortClick);
  button.attachLongPressStart(onLongPressStart);
  button.attachLongPressStop(onLongPressStop);
  button.setDebounceTicks(30);

  displayState();
}

// --- Main Loop ---
void loop() {
  button.tick();
  updateRelay();
  displayState();

  if (editMode && (millis() - lastEditTime > EDIT_TIMEOUT)) {
    editMode = false;
    editingOnTime = true;
    editingHours = true;
    displayState();
  }

  delay(10);
}
