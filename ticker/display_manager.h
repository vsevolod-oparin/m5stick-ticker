#pragma once

#include <stdarg.h>
#include <M5GFX.h>

#include "price_update.h"

M5GFX display;
M5Canvas canvas(&display);

struct DisplayManager {
  bool debug_mode;
  DisplayManager(bool debug_mode = false): debug_mode(debug_mode) {}

  void setup();

  void set_log_mode();
  void info(char const* format_string, ...);
  void debug(char const* format_string, ...);

  void ticker_image(PriceUpdate const& price_update);
};


// IMPLEMENTATION

void DisplayManager::setup() {
  display.begin();

  if (display.isEPD()) {
    display.setEpdMode(epd_mode_t::epd_fastest);
    display.invertDisplay(true);
    display.clear(TFT_BLACK);
  }
  if (display.width() < display.height()) {
    display.setRotation(display.getRotation() ^ 1);
  }
  
  canvas.setColorDepth(8); // mono color
  canvas.createSprite(display.width(), display.height());
}

void DisplayManager::set_log_mode() {
  canvas.clear(TFT_BLACK);
  canvas.setTextSize(1.0);
  canvas.setFont(&fonts::Font0);
  canvas.setTextColor(TFT_LIGHTGREY);
  canvas.setTextSize((float)canvas.width() / 120);
  canvas.setTextScroll(true);
  canvas.setCursor(0, 0);
}

void DisplayManager::info(char const* format_string, ...) {
  va_list args;
  va_start(args, format_string);
  canvas.vprintf(format_string, args);
  canvas.pushSprite(0, 0);
  va_end(args);
}

void DisplayManager::debug(char const* format_string, ...) {
  if (!debug_mode) {
    return;
  }
  va_list args;
  va_start(args, format_string);
  canvas.printf("[DEBUG] ");
  canvas.vprintf(format_string, args);
  canvas.pushSprite(0, 0);
  va_end(args);
}

void DisplayManager::ticker_image(PriceUpdate const& price_update) {
  canvas.startWrite();
  canvas.setClipRect(0, 0, display.width(), display.height());

  canvas.clear(TFT_BLACK);

  canvas.setTextSize(2.5);
  canvas.setFont(&fonts::Font8x8C64);
  canvas.setTextColor(TFT_WHITE);
  canvas.setTextDatum(textdatum_t::top_left);
  canvas.drawString(price_update.ticker.c_str(), 10, 10);

  float font_scale = 1.0;
  canvas.setFont(&fonts::FreeSans24pt7b);
  
  canvas.setTextSize(1.0 * font_scale);
  canvas.setTextColor(TFT_WHITE);
  canvas.setTextDatum(textdatum_t::middle_center);
  canvas.drawFloat(price_update.current_price, 2, display.width() / 2, display.height() / 2);

  if (price_update.day_change < -0.005) {
    canvas.setTextColor(TFT_RED);
  } else if (price_update.day_change > 0.005) {
    canvas.setTextColor(TFT_GREEN);
  } else {
    canvas.setTextColor(TFT_LIGHTGREY);
  }

  canvas.setTextSize(0.6 * font_scale);
  char day_change_percentage_str[80];
  sprintf(day_change_percentage_str, "%0.2f%%", price_update.day_change_percentage);
  
  canvas.setTextDatum(textdatum_t::bottom_left);
  canvas.drawFloat(price_update.day_change, 2, 10, display.height() - 8);
  
  canvas.setTextDatum(textdatum_t::bottom_right);
  canvas.drawString(day_change_percentage_str,  display.width() - 8, display.height() - 8);
  
  canvas.clearClipRect();
  canvas.endWrite();

  canvas.pushSprite(0, 0);
}
