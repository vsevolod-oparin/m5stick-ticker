#pragma once

struct PriceUpdate {
  String  ticker;

  float   current_price;
  float   day_change;
  float   day_change_percentage;
};
