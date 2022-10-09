#pragma once

#include <AsyncHTTPSRequest_Generic.h>  

#include "display_manager.h"
#include "network_fetcher.h"
#include "price_update.h"

struct TickerTracker {
  typedef std::function<void(void*, AsyncHTTPSRequest*, int readyState)> AsyncCallback;
  
  String              ticker;
  AsyncHTTPSRequest   request;
  PriceUpdate         price_update;
  int                 update_time_offset;

  DisplayManager*     display_manager;
  NetworkFetcher*     network_fetcher;
  AsyncCallback       request_callback;

  explicit TickerTracker() {}
  
  explicit TickerTracker(
      String const& ticker,
      DisplayManager* display_manager,
      NetworkFetcher* network_fetcher) :
    ticker(ticker),
    update_time_offset(random(FETCH_UPDATE_INTERVAL)),
    display_manager(display_manager),
    network_fetcher(network_fetcher) {
  }

  void network_setup() {
    using namespace std::placeholders;
    
    display_manager->info("Fetching %s...\n", ticker.c_str()); 
    request_callback = std::bind(&NetworkFetcher::request_callback, network_fetcher, _1, _2, _3);
    price_update = network_fetcher->http_fetch(ticker);
    request.onReadyStateChange(request_callback, (void*)&price_update);
    request.setDebug(false);
  }

  void fetch_update() {
    update_time_offset += QUICK_UPDATE_INTERVAL;
    if (update_time_offset > FETCH_UPDATE_INTERVAL) {
      update_time_offset -= FETCH_UPDATE_INTERVAL;
      network_fetcher->async_http_fetch(ticker, request);
    }
  }
};
