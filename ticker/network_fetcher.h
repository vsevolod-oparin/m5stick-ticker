#pragma once

#include <ArduinoJson.h>
#include <AsyncHTTPSRequest_Generic.h>  
#include <HTTPClient.h>
#include <WiFi.h>

#include "display_manager.h"
#include "price_update.h"
#include "secrets.h"

PriceUpdate parse_payload(String const& ticker, String const& payload);

struct NetworkFetcher {
  DisplayManager* display_manager;
  bool            use_wifi;

  explicit    NetworkFetcher(DisplayManager* display_manager, bool use_wifi = true);
  void        setup();
  
  String      quote_url(char const* ticker, char const* api_token);
  
  PriceUpdate http_fetch(String const& ticker);
  void        async_http_fetch(String const& ticker, AsyncHTTPSRequest* request);
  void        request_callback(void* opt_param, AsyncHTTPSRequest* request, int readyState);
  
};


// IMPLEMENTATION

NetworkFetcher::NetworkFetcher(
    DisplayManager* display_manager,
    bool use_wifi):
  display_manager(display_manager),
  use_wifi(use_wifi) {}

void NetworkFetcher::setup() {
  if (!use_wifi) {
    display_manager->info("WiFi is switched off");
    return;
  }
  
  display_manager->set_log_mode();
  display_manager->info("Connecting to %s ", wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display_manager->info(".");
  }
  display_manager->info(" CONNECTED\n");
}

String NetworkFetcher::quote_url(char const* ticker, char const* api_token) {
  String prefix        = String("https://finnhub.io/api/v1/quote");
  String ticker_arg    = String("symbol=") + String(ticker);
  String api_token_arg = String("token=") + String(api_token);

  return prefix + String('?') + ticker_arg + String('&') + api_token_arg;
}

PriceUpdate parse_payload(String const& ticker, String const& payload) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload.c_str());
  return PriceUpdate {
    .ticker                 = ticker,
    .current_price          = doc["c"].as<float>(),
    .day_change             = doc["d"].as<float>(),
    .day_change_percentage  = doc["dp"].as<float>()
  };
}

void NetworkFetcher::async_http_fetch(String const& ticker, AsyncHTTPSRequest* request) {
  if (request->readyState() == readyStateUnsent || request->readyState() == readyStateDone) {
    String quote_url_str = quote_url(ticker.c_str(), api_token);
    bool requestOpenResult = request->open("GET", quote_url_str.c_str());
    
    if (requestOpenResult) {
      request->send();
    } else {
      display_manager->debug("Can't send bad request\n"); 
    }
  } else {
    display_manager->debug("Can't send request\n"); 
  }
}

PriceUpdate NetworkFetcher::http_fetch(String const& ticker) {
  PriceUpdate price_update;

  display_manager->debug("[HTTP] begin...\n");
  HTTPClient http;

  // configure traged server and url
  String quote_url_str = quote_url(ticker.c_str(), api_token);
  http.begin(quote_url_str.c_str()); //HTTP

  // start connection and send HTTP header
  display_manager->debug("[HTTP] GET...\n");
  int httpCode = http.GET();

  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    display_manager->debug("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      display_manager->debug("Payload\n%s\n", payload.c_str());

      price_update = parse_payload(ticker, payload);
    }
  } else {
    display_manager->debug("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  return price_update;
}

void NetworkFetcher::request_callback(void* opt_param, AsyncHTTPSRequest* request, int readyState) {
  PriceUpdate* price_update_ptr = (PriceUpdate*)opt_param;
  HTTPClient http;
  if (readyState == readyStateDone) {
    int httpCode = request->responseHTTPcode();
    if (httpCode == HTTP_CODE_OK) {
      *price_update_ptr = parse_payload(price_update_ptr->ticker, request->responseText());
    }
  }
}
