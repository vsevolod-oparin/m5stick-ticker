#include <M5StickC.h>

#define DISPLAY_MANAGER_DEBUG false 

#include "constants.h"
#include "display_manager.h"
#include "network_fetcher.h"
#include "price_update.h"
#include "ticker_tracker.h"

String        tickers[] = {
  "TSLA", 
  "XOM"
};

const int     TICKER_NUM = sizeof(tickers) / sizeof(String);
TickerTracker trackers[TICKER_NUM];
int           ticker_iterator = 0;


DisplayManager display_manager(DISPLAY_MANAGER_DEBUG);
NetworkFetcher network_fetcher(&display_manager);

void init_prices() {
  for (size_t i = 0; i < TICKER_NUM; ++i) {
    trackers[i] = TickerTracker(tickers[i], &display_manager, &network_fetcher);
    trackers[i].network_setup();
  }
}

void setup() {
  M5.begin();
  display_manager.setup();
  network_fetcher.setup();
  init_prices();
}

void interface_update() {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    ticker_iterator = (ticker_iterator + 1) % TICKER_NUM;
  }
  if (M5.BtnB.wasPressed()) {
    ticker_iterator = (ticker_iterator + TICKER_NUM - 1) % TICKER_NUM;
  }
  display_manager.ticker_image(trackers[ticker_iterator].price_update);
}

void price_set_update() {
  for (auto i = 0; i < TICKER_NUM; ++i) {
    trackers[i].fetch_update();
  }
}

void main_loop() {
  interface_update();
  price_set_update();
  delay(QUICK_UPDATE_INTERVAL);
}

void loop() {
  main_loop();
}
