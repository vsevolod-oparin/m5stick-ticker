#include <M5StickC.h>

#include "network_fetcher.h"
#include "price_update.h"
#include "display_manager.h"

const int quick_update_interval = 20;
const int fetch_update_interval = 10 * 1000;
const int TICKER_NUM = 2;

String tickers[TICKER_NUM] = { 
  "TSLA", 
  "XOM"
};

DisplayManager display_manager;
NetworkFetcher network_fetcher(&display_manager);

PriceUpdate price_updates[TICKER_NUM];
int         update_time_offsets[TICKER_NUM];
int         ticker_iterator = 0;

void init_prices();

void setup() {
  M5.begin();
  display_manager.setup();
  network_fetcher.setup();
  init_prices();
}

void init_prices() {
  for (size_t i = 0; i < TICKER_NUM; ++i) {
    display_manager.info("Fetching %s...\n", tickers[i].c_str());
    price_updates[i] = network_fetcher.http_fetch(tickers[i]);
    update_time_offsets[i] = random(fetch_update_interval);
  } 
}

void interface_update() {
  display_manager.ticker_image(price_updates[ticker_iterator]);
  M5.update();
  if (M5.BtnA.wasPressed()) {
    ticker_iterator = (ticker_iterator + 1) % TICKER_NUM;
    display_manager.ticker_image(price_updates[ticker_iterator]);
  }
}

void price_set_update() {
  for (size_t i = 0; i < TICKER_NUM; ++i) {
    update_time_offsets[i] += quick_update_interval;
    if (update_time_offsets[i] > fetch_update_interval) {
      update_time_offsets[i] -= fetch_update_interval;
      price_updates[i] = network_fetcher.http_fetch(tickers[i]);
    }
    if (i == ticker_iterator) {
      display_manager.ticker_image(price_updates[ticker_iterator]);
    }
  }
}


void main_loop() {
  interface_update();
  price_set_update();
}

void loop() {
  main_loop();
}
