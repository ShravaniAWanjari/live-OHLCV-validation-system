#pragma once

#include "simdjson.h"
#include "types.hpp"
#include <charconv>
#include <iostream>
#include <string_view>
#include <system_error>

class Ingestor {
public:
  Ingestor() {
    auto err = parser_.allocate(1024 * 1024);
    if (err) {
      std::cerr << "Allocation failed: " << err << std::endl;
    }
  }

  bool parse_kline(const std::string &raw_json, TickData &out_tick) {
    try {
      simdjson::dom::element doc = parser_.parse(raw_json);
      auto k = doc["k"];

      out_tick.open = parse_double(k["o"]);
      out_tick.high = parse_double(k["h"]);
      out_tick.low = parse_double(k["l"]);
      out_tick.close = parse_double(k["c"]);
      out_tick.volume = parse_double(k["v"]);

      out_tick.exchange_timestamp = k["t"].get_uint64();

      return true;
    } catch (const std::exception &e) {
      std::cerr << "Parsing error: " << e.what() << std::endl;
      return false;
    }
  }

private:
  simdjson::dom::parser parser_;

  double parse_double(simdjson::dom::element el) {
    std::string_view sv = el.get_string().value();
    double val = 0.0;

    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
    if (ec != std::errc()) {
      return 0.0;
    }

    return val;
  }
};
